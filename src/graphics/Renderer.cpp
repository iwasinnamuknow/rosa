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
 *  You should have received a copy of the GNU General Public License along with bbai. If not,
 *  see <https://www.gnu.org/licenses/>.
 */

#include <cstddef>
#include <cstdint>
#include <graphics/Renderer.hpp>
#include <graphics/gl.hpp>
#include <GLFW/glfw3.h>
#include <tracy/Tracy.hpp>

namespace rosa {

    Renderer::Renderer() {

        // See destructor for deletion
        m_vertex_buffer = new Vertex[max_vertex_count];
        m_vertex_buffer_ptr = m_vertex_buffer;

        m_pid = glCreateProgram();

        // Setup opengl
        glGenVertexArrays(1, &m_vao);
        glGenBuffers(1, &m_vbo);

        glBindVertexArray(m_vao);
        glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
        //glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * (m_quads * 4), nullptr, GL_DYNAMIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, position.x));

        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_TRUE, sizeof(Vertex), (const void*)offsetof(Vertex, texture_coords.x));

        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, colour.r));

        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, texture_slot));

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

        // Generate empty texture 1x1 white
//        glGenTextures(1, &m_empty_tex_id);
//        glBindTexture(GL_TEXTURE_2D, m_empty_tex_id);
//        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
//        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
//        uint32_t colour = 0xffffffff;
//        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, &colour);
//        m_textures[0] = m_empty_tex_id;

        linkShaders();

        m_mvp_id = glGetUniformLocation(m_pid, "mvp");
    }

    Renderer::~Renderer() {
        
        glDeleteVertexArrays(1, &m_vao);
        glDeleteBuffers(1, &m_vbo);
        glDeleteBuffers(1, &m_ibo);

        glDeleteTextures(1, &m_empty_tex_id);

        delete[] m_vertex_buffer;
    }

    auto Renderer::submitForBatch(const Quad& quad) -> void {
        ZoneScopedN("Render:Renderer:SubmitForBatch");

        if ((m_index_count) + 6 > max_index_count || m_texture_count > 31) {
            flushBatch();
        }

        float texture_index{0.F};
        for (uint32_t i = 1; i < m_texture_count; i++) {
            if (m_textures[i] == quad.texture_id) {
                texture_index = static_cast<float>(i);
                break;
            }
        }

        if (texture_index == 0.F) {
            texture_index = static_cast<float>(m_texture_count);
            m_textures[m_texture_count] = quad.texture_id;
            m_texture_count++;
        }

        m_vertex_buffer_ptr->position = quad.pos;
        m_vertex_buffer_ptr->colour = quad.colour;
        m_vertex_buffer_ptr->texture_coords = quad.texture_rect_pos;
        m_vertex_buffer_ptr->texture_slot = texture_index;
        m_vertex_buffer_ptr++;

        m_vertex_buffer_ptr->position = { quad.pos.x + quad.size.x, quad.pos.y };
        m_vertex_buffer_ptr->colour = quad.colour;
        m_vertex_buffer_ptr->texture_coords = {quad.texture_rect_pos.x + quad.texture_rect_size.x, quad.texture_rect_pos.y};
        m_vertex_buffer_ptr->texture_slot = texture_index;
        m_vertex_buffer_ptr++;

        m_vertex_buffer_ptr->position = { quad.pos.x, quad.pos.y + quad.size.y };
        m_vertex_buffer_ptr->colour = quad.colour;
        m_vertex_buffer_ptr->texture_coords = {quad.texture_rect_pos.x, quad.texture_rect_pos.y + quad.texture_rect_size.y};
        m_vertex_buffer_ptr->texture_slot = texture_index;
        m_vertex_buffer_ptr++;

        m_vertex_buffer_ptr->position = quad.pos + quad.size;
        m_vertex_buffer_ptr->colour = quad.colour;
        m_vertex_buffer_ptr->texture_coords = quad.texture_rect_pos + quad.texture_rect_size;
        m_vertex_buffer_ptr->texture_slot = texture_index;
        m_vertex_buffer_ptr++;

        m_index_count += 6;
        m_quad_draws++;
    }

    auto Renderer::submit(const Quad& quad, glm::mat4 transform, bool override_mvp) -> void {
        ZoneScopedN("Render:Renderer:Submit");

        float texture_index{0.F};
        for (uint32_t i = 1; i < m_texture_count; i++) {
            if (m_textures[i] == quad.texture_id) {
                texture_index = static_cast<float>(i);
                break;
            }
        }

        if (texture_index == 0.F) {
            texture_index = static_cast<float>(m_texture_count);
            m_textures[m_texture_count] = quad.texture_id;
            m_texture_count++;
        }

        std::array<Vertex, 4> vertices;

        vertices[0].position = glm::vec2(-(quad.size.x/2), -(quad.size.y/2));
        vertices[0].colour = quad.colour;
        vertices[0].texture_coords = glm::vec2(quad.texture_rect_pos.x, quad.texture_rect_pos.y + quad.texture_rect_size.y);
        vertices[0].texture_slot = texture_index;

        vertices[1].position = glm::vec2(  quad.size.x/2,  -(quad.size.y/2));
        vertices[1].colour = quad.colour;
        vertices[1].texture_coords = (quad.texture_rect_pos + quad.texture_rect_size);
        vertices[1].texture_slot = texture_index;

        vertices[2].position = glm::vec2(-(quad.size.x/2),   quad.size.y/2);
        vertices[2].colour = quad.colour;
        vertices[2].texture_coords = quad.texture_rect_pos;
        vertices[2].texture_slot = texture_index;

        vertices[3].position = glm::vec2(  quad.size.x/2,    quad.size.y/2);
        vertices[3].colour = quad.colour;
        vertices[3].texture_coords = glm::vec2(quad.texture_rect_pos.x + quad.texture_rect_size.x, quad.texture_rect_pos.y);
        vertices[3].texture_slot = texture_index;

        m_quad_draws++;

        glm::mat4 mvp_tmp;
        if (override_mvp) {
            mvp_tmp = transform;
        } else {
            mvp_tmp = m_mvp * transform;
        }

        glUseProgram(m_pid);
        glBindVertexArray(m_vao);
        glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * 4, vertices.data(), GL_DYNAMIC_DRAW);

        for (auto texture_id : m_textures) {
            if (texture_id != 0) {
                glBindTexture(GL_TEXTURE_2D, texture_id);
                m_texture_binds++;
            }
        }

        glUniformMatrix4fv(m_mvp_id, 1, GL_FALSE, &mvp_tmp[0][0]);

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

        glUseProgram(0);

        m_texture_count = 1;

        m_draw_calls++;
    }

    auto Renderer::flushBatch() -> void {
        ZoneScopedN("Render:Renderer:FlushBatch");
        
        glUseProgram(m_pid);
        glBindVertexArray(m_vao);
        glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * (m_quad_draws * 4), m_vertex_buffer, GL_DYNAMIC_DRAW);

        for (auto texture_id : m_textures) {
            if (texture_id != 0) {
                glBindTexture(GL_TEXTURE_2D, texture_id);
                m_texture_binds++;
            }
        }

        glUniformMatrix4fv(m_mvp_id, 1, GL_FALSE, &m_mvp[0][0]);

        glDrawElements(GL_TRIANGLES, m_index_count, GL_UNSIGNED_INT, nullptr);

        glUseProgram(0);

        m_index_count = 0;
        m_texture_count = 1;
        m_vertex_buffer_ptr = m_vertex_buffer;

        m_draw_calls++;
    }

    auto Renderer::updateMvp(glm::mat4 projection) -> void {
        m_mvp = projection;
    }

    auto Renderer::linkShaders() -> void {

        bool defaulted_v_shader{false};
        bool defaulted_f_shader{false};

        if (m_vertex_shader == nullptr){ 
            m_vertex_shader = new Shader("default", Uuid(), "", VertexShader);
            defaulted_v_shader = true;
            spdlog::warn("Using default vertex shader");
        }
        if (m_fragment_shader == nullptr) {
            m_fragment_shader = new Shader("default", Uuid(), "", FragmentShader);
            defaulted_f_shader = true;
            spdlog::warn("Using default fragment shader");
        }

        auto vshader_id = glCreateShader(VertexShader);
        auto fshader_id = glCreateShader(FragmentShader);

        GLint result = GL_FALSE;
        int info_log_length{0};

        // Compile vertex shader
        spdlog::debug("Compiling vertex shader");
        const char* vertex_source_ptr = m_vertex_shader->getSource().c_str();
        glShaderSource(vshader_id, 1, &vertex_source_ptr , nullptr);
        glCompileShader(vshader_id);

        // Check shader
        glGetShaderiv(vshader_id, GL_COMPILE_STATUS, &result);
        glGetShaderiv(vshader_id, GL_INFO_LOG_LENGTH, &info_log_length);
        if ( info_log_length > 0 ){
            std::vector<char> error(info_log_length + 1);
            glGetShaderInfoLog(vshader_id, info_log_length, nullptr, error.data());
            spdlog::error("Failed to compile shader: {}", error.data());
        }
        
        // Compile fragment shader
        spdlog::debug("Compiling fragment shader");
        const char* frag_source_ptr = m_fragment_shader->getSource().c_str();
        glShaderSource(fshader_id, 1, &frag_source_ptr , nullptr);
        glCompileShader(fshader_id);

        // Check shader
        glGetShaderiv(fshader_id, GL_COMPILE_STATUS, &result);
        glGetShaderiv(fshader_id, GL_INFO_LOG_LENGTH, &info_log_length);
        if ( info_log_length > 0 ){
            std::vector<char> error(info_log_length + 1);
            glGetShaderInfoLog(fshader_id, info_log_length, nullptr, error.data());
            spdlog::error("Failed to compile shader: {}", error.data());
        }

        // Link shader to program
        spdlog::debug("Linking shaders");
        glAttachShader(m_pid, vshader_id);
        glAttachShader(m_pid, fshader_id);
        glLinkProgram(m_pid);

        // Check the program
        glGetProgramiv(m_pid, GL_LINK_STATUS, &result);
        glGetProgramiv(m_pid, GL_INFO_LOG_LENGTH, &info_log_length);
        if ( info_log_length > 0 ){
            std::vector<char> error(info_log_length + 1);
            glGetProgramInfoLog(m_pid, info_log_length, NULL, error.data());
            spdlog::error("Failed to link shaders: {}", error.data());
        }

        glDetachShader(m_pid, vshader_id);
        glDetachShader(m_pid, fshader_id);
        glDeleteShader(vshader_id);
        glDeleteShader(fshader_id);

        if (defaulted_v_shader) {
            delete m_vertex_shader;
        }

        if (defaulted_f_shader) {
            delete m_fragment_shader;
        }
    }

    auto Renderer::getStats() -> RendererStats {
        return {m_draw_calls, m_quad_draws * 4, m_texture_binds};
    }

    auto Renderer::clearStats() -> void {
        m_draw_calls = 0;
        m_quad_draws = 0;
        m_texture_binds = 0;
    }

    auto Renderer::setShader(ShaderType type, Shader* shader) -> void {
        if (type == ShaderType::FragmentShader && shader != nullptr) {
            m_fragment_shader = shader;
        }
    }

} // namespace rosa