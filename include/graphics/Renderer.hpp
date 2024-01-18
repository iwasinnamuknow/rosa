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
#include <graphics/Vertex.hpp>
#include <graphics/Quad.hpp>
#include <graphics/Shader.hpp>
#include <graphics/RenderWindow.hpp>
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

    class Renderer {
        public:
            static auto getInstance() -> Renderer&;
            static auto shutdown() -> void;

            auto submitForBatch(const Quad& quad) -> void;
            auto flushBatch() -> void;

            auto submit(const Quad& quad, glm::mat4 transform) -> void;

            auto updateMvp(glm::mat4 projection) -> void;
            auto getStats() -> RendererStats;
            auto clearStats() -> void;

            auto setShader(ShaderType type, Shader* shader) -> void;
            auto linkShaders() -> void;

            Renderer();
            ~Renderer();

        private:

            Vertex* m_vertex_buffer;
            Vertex* m_vertex_buffer_ptr;
            GLuint m_vao;
            GLuint m_vbo;
            GLuint m_ibo;
            
            int m_index_count{0};
            std::array<uint32_t, max_index_count> m_indices;

            uint32_t m_texture_count{1};
            std::array<uint32_t, max_textures> m_textures;

            uint32_t m_empty_tex_id{0};

            // statistics per call
            int m_quad_draws{0};
            int m_draw_calls{0};
            int m_texture_binds{0};

            unsigned int m_pid;

            Shader* m_vertex_shader{nullptr};
            Shader* m_fragment_shader{nullptr};

            glm::mat4 m_mvp{0.F};
            int m_mvp_id;

            static std::unique_ptr<Renderer> s_instance;
    };

} // namespace rosa