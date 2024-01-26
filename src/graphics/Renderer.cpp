/*
 * This file is part of rosa.
 *
 *  rosa is free software: you can redistribute it and/or modify it under the terms of the
 *  GNU General Public License as published by the Free Software Foundation, either version
 *  3 of the License, or (at your option) any later version.
 *
 *  rosa is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without
 *  even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along with rosa. If not,
 *  see <https://www.gnu.org/licenses/>.
 */

#include <GLFW/glfw3.h>
#include <cstddef>
#include <cstdint>
#include <graphics/Renderer.hpp>
#include <graphics/gl.hpp>
#include <tracy/Tracy.hpp>

namespace rosa {

    static bool sortByScreenSpace(const Renderable& a, const Renderable& b) {
        return a.screen_space < b.screen_space;
    }

    static bool sortByShader(const Renderable& a, const Renderable& b) {
        return a.shader_program < b.shader_program;
    }

    Renderer::Renderer() {

        // See destructor for deletion
        m_vertex_buffer     = new Vertex[max_vertex_count];
        m_vertex_buffer_ptr = m_vertex_buffer;

        // Setup opengl
        glGenVertexArrays(1, &m_vao);
        glGenBuffers(1, &m_vbo);

        glBindVertexArray(m_vao);
        glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
        //glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * (m_quads * 4), nullptr, GL_DYNAMIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*) offsetof(Vertex, position.x));

        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_TRUE, sizeof(Vertex), (const void*) offsetof(Vertex, texture_coords.x));

        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*) offsetof(Vertex, colour.r));

        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*) offsetof(Vertex, texture_slot));

        // Fill the index array
        uint32_t offset{0};
        for (int i = 0; i < max_index_count; i += 6) {
            m_indices[i + 0] = 0 + offset;
            m_indices[i + 1] = 1 + offset;
            m_indices[i + 2] = 2 + offset;

            m_indices[i + 3] = 1 + offset;
            m_indices[i + 4] = 2 + offset;
            m_indices[i + 5] = 3 + offset;

            offset += 4;
        }

        m_textures.fill(0);

        glGenBuffers(1, &m_ibo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * max_index_count, m_indices.data(), GL_STATIC_DRAW);
    }

    Renderer::~Renderer() {

        glDeleteVertexArrays(1, &m_vao);
        glDeleteBuffers(1, &m_vbo);
        glDeleteBuffers(1, &m_ibo);

        delete[] m_vertex_buffer;
    }

    auto Renderer::makeShaderProgram(Uuid vertex_shader, Uuid fragment_shader) -> ShaderProgram* {
        for (const auto& program: m_shaders) {
            if (program->getVertexShaderUuid() == vertex_shader && program->getFragmentShaderUuid() == fragment_shader) {
                return program.get();
            }
        }

        auto program = m_shaders.emplace(m_shaders.end(), std::make_unique<ShaderProgram>(vertex_shader, fragment_shader));
        program->get()->compile();

        return program->get();
    }

    auto Renderer::submit(Renderable renderable) -> void {
        ZoneScopedN("Render:Renderer:Submit");

        assert(renderable.shader_program->isCompiled());

        if (m_renderables.size() >= max_quad_count || m_texture_count > 31) {
            flushBatch();
        }

        //float texture_index{0.F};
        for (uint32_t i = 1; i < m_texture_count; i++) {
            if (m_textures[i] == renderable.quad.texture_id) {
                renderable.texture_index = static_cast<float>(i);
                break;
            }
        }

        if (renderable.texture_index == 0.F) {
            renderable.texture_index    = static_cast<float>(m_texture_count);
            m_textures[m_texture_count] = renderable.quad.texture_id;
            m_texture_count++;
        }

        m_renderables.push_back(renderable);
    }

    auto Renderer::flushBatch() -> void {
        ZoneScopedN("Render:Renderer:FlushBatch");

        // bind active textures
        int i{0};
        for (auto texture_id: m_textures) {
            if (texture_id != 0) {
                glActiveTexture(GL_TEXTURE0 + i);
                glBindTexture(GL_TEXTURE_2D, texture_id);
                m_texture_binds++;
                i++;
            }
        }

        // sort renderables by world space first, then screen space
        auto ss_group_start = m_renderables.begin();
        while (ss_group_start != m_renderables.end()) {
            auto ss_group_end = std::upper_bound(
                    ss_group_start, m_renderables.end(),
                    *ss_group_start, sortByScreenSpace);

            // Within this, sort by shader program
            std::sort(ss_group_start, ss_group_end, sortByShader);

            ss_group_start = ss_group_end;
        }

        // If we're rendering in screen space, use an orthographic projection of the screen
        // If it's world space, use the regular MVP

        unsigned int current_shader_id{0};
        glm::mat4    current_transform{1.F};
        int          current_mvp_id{-1};
        glm::mat4    current_mvp{m_view_matrix * m_projection_matrix};
        bool         screen_space{false};

        for (auto& renderable: m_renderables) {

            // Check for the switch between world and screen space
            if (!screen_space && renderable.screen_space) {
                if (m_index_count > 0) {
                    flush(current_shader_id, current_mvp, current_mvp_id);
                }
                current_mvp  = m_projection_matrix;
                screen_space = true;
            }

            // Whenever we encounter a different shader program, flush the cache (assuming there is
            // anything in there).
            if (renderable.shader_program->getProgramId() != current_shader_id) {
                if (m_index_count > 0) {
                    flush(current_shader_id, current_mvp, current_mvp_id);
                }
                current_shader_id = renderable.shader_program->getProgramId();
                current_mvp_id    = renderable.shader_program->getMvpId();
                m_shader_changes++;
            }

            // Calculate the positions for each of the 4 vertices, taking the object transform
            // into account.
            glm::vec2 top_left = renderable.transform *
                                 glm::vec4(-(renderable.quad.size.x / 2.F),
                                           -(renderable.quad.size.y / 2.F),
                                           1.F, 1.F);

            glm::vec2 top_right = renderable.transform *
                                  glm::vec4((renderable.quad.size.x / 2.F),
                                            -(renderable.quad.size.y / 2.F),
                                            1.F, 1.F);

            glm::vec2 bottom_left = renderable.transform *
                                    glm::vec4(-(renderable.quad.size.x / 2.F),
                                              (renderable.quad.size.y / 2.F),
                                              1.F, 1.F);

            glm::vec2 bottom_right = renderable.transform *
                                     glm::vec4((renderable.quad.size.x / 2.F),
                                               (renderable.quad.size.y / 2.F),
                                               1.F, 1.F);

            // Populate the vertex cache with data from the quad
            m_vertex_buffer_ptr->position       = top_left;
            m_vertex_buffer_ptr->colour         = renderable.quad.colour;
            m_vertex_buffer_ptr->texture_coords = renderable.quad.texture_rect_pos;
            m_vertex_buffer_ptr->texture_slot   = renderable.texture_index;
            m_vertex_buffer_ptr++;

            m_vertex_buffer_ptr->position       = top_right;
            m_vertex_buffer_ptr->colour         = renderable.quad.colour;
            m_vertex_buffer_ptr->texture_coords = {renderable.quad.texture_rect_pos.x + renderable.quad.texture_rect_size.x, renderable.quad.texture_rect_pos.y};
            m_vertex_buffer_ptr->texture_slot   = renderable.texture_index;
            m_vertex_buffer_ptr++;

            m_vertex_buffer_ptr->position       = bottom_left;
            m_vertex_buffer_ptr->colour         = renderable.quad.colour;
            m_vertex_buffer_ptr->texture_coords = {renderable.quad.texture_rect_pos.x, renderable.quad.texture_rect_pos.y + renderable.quad.texture_rect_size.y};
            m_vertex_buffer_ptr->texture_slot   = renderable.texture_index;
            m_vertex_buffer_ptr++;

            m_vertex_buffer_ptr->position       = bottom_right;
            m_vertex_buffer_ptr->colour         = renderable.quad.colour;
            m_vertex_buffer_ptr->texture_coords = renderable.quad.texture_rect_pos + renderable.quad.texture_rect_size;
            m_vertex_buffer_ptr->texture_slot   = renderable.texture_index;
            m_vertex_buffer_ptr++;

            m_index_count += 6;
            m_quad_draws++;
        }

        // Flush the cache one last time if there is anything to render
        if (m_index_count > 0) {
            flush(current_shader_id, current_mvp, current_mvp_id);
        }

        // Clear the render queue
        m_renderables.clear();
    }

    auto Renderer::flush(unsigned int shader_program_id, glm::mat4 mvp, int mvp_id) -> void {
        ZoneScopedN("Render:Renderer:Flush");

        glUseProgram(shader_program_id);
        glBindVertexArray(m_vao);
        glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * (m_quad_draws * 4), m_vertex_buffer, GL_DYNAMIC_DRAW);

        glUniformMatrix4fv(mvp_id, 1, GL_FALSE, &mvp[0][0]);

        glDrawElements(GL_TRIANGLES, m_index_count, GL_UNSIGNED_INT, nullptr);

        m_index_count       = 0;
        m_texture_count     = 0;
        m_vertex_buffer_ptr = m_vertex_buffer;

        m_draw_calls++;
    }

    auto Renderer::getStats() -> RendererStats {
        return {m_draw_calls, m_quad_draws * 4, m_texture_binds, m_shader_changes};
    }

    auto Renderer::clearStats() -> void {
        m_draw_calls     = 0;
        m_quad_draws     = 0;
        m_texture_binds  = 0;
        m_shader_changes = 0;
    }

    std::unique_ptr<Renderer> Renderer::s_instance{nullptr};

    auto Renderer::getInstance() -> Renderer& {
        if (!s_instance) {
            s_instance = std::make_unique<Renderer>();
        }

        return *s_instance;
    }

    auto Renderer::updateVp(glm::mat4 view, glm::mat4 projection) -> void {
        m_view_matrix       = view;
        m_projection_matrix = projection;
    }

    auto Renderer::shutdown() -> void {
        if (s_instance) {
            s_instance.release();
            s_instance.reset();
        }
    }

}// namespace rosa