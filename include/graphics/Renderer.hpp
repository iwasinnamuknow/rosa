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

#pragma once

#include <cstdint>
#include <graphics/Quad.hpp>
#include <graphics/RenderWindow.hpp>
#include <graphics/ShaderProgram.hpp>
#include <graphics/Vertex.hpp>
#include <memory>

constexpr int max_vertex_count{10000};
constexpr int max_quad_count{max_vertex_count / 4};
constexpr int max_index_count{max_quad_count * 6};
constexpr int max_textures{32};

namespace rosa {

    struct RendererStats {
        int draws{0};
        int verts{0};
        int textures{0};
    };

    struct Renderable {
        Quad           quad;
        glm::mat4      transform{1.F};
        ShaderProgram* shader_program{};
        bool           screen_space{false};
        float          texture_index{0.F};
    };

    class Renderer {
        public:
            static auto getInstance() -> Renderer&;
            static auto shutdown() -> void;

            auto submit(Renderable renderable) -> void;
            auto flushBatch() -> void;

            auto getStats() -> RendererStats;
            auto clearStats() -> void;

            Renderer();
            ~Renderer();

            auto updateVp(glm::mat4 view, glm::mat4 projection) -> void {
                m_view_matrix       = view;
                m_projection_matrix = projection;
            }

            auto makeShaderProgram(Uuid vertex_shader = {}, Uuid fragment_shader = {}) -> ShaderProgram*;

        private:
            auto flush(unsigned int shader_program_id, glm::mat4 mvp, int mvp_id) -> void;

            std::vector<Renderable> m_renderables{};

            Vertex* m_vertex_buffer;
            Vertex* m_vertex_buffer_ptr;
            GLuint m_vao;
            GLuint m_vbo;
            GLuint m_ibo;
            
            int m_index_count{0};
            std::array<uint32_t, max_index_count> m_indices;

            uint32_t m_texture_count{0};
            std::array<uint32_t, max_textures> m_textures;

            uint32_t m_empty_tex_id{0};

            // statistics per call
            int m_quad_draws{0};
            int m_draw_calls{0};
            int m_texture_binds{0};

            glm::vec4 m_camera_pos{0};

            glm::mat4 m_view_matrix;
            glm::mat4 m_projection_matrix;

            std::vector<std::unique_ptr<ShaderProgram>> m_shaders{};

            static std::unique_ptr<Renderer> s_instance;
    };

} // namespace rosa