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

    /**
     * \brief Stats for each frame
     */
    struct RendererStats {
        int draws{0};
        int vertices{0};
        int textures{0};
        int shaders{0};
    };

    /**
     * \brief Collection of data required to render
     */
    struct Renderable {
        Quad           quad;
        glm::mat4      transform{1.F};
        ShaderProgram* shader_program{};
        bool           screen_space{false};
        float          texture_index{0.F};
    };

    /**
     * \brief A semi-intelligent batch quad renderer
     *
     * Renderables are submitted to the renderer. On flushing, the renderer will
     * sort renderables by render space. World-space objects will be rendered first,
     * then screen-space.
     *
     * The renderables are sorted secondarily by shader, grouping objects which use the
     * same shader together to minimise program changes.
     *
     * There is an upper limit to how many objects will be rendered in a single draw call.
     * If the limit is reached and additional objects are submitted, the renderer will flush
     * the render queue before adding the new object.
     */
    class Renderer {
    public:
        /**
         * \brief Get the global instance
         */
        static auto getInstance() -> Renderer&;

        /**
         * \brief Cleanup the global instance
         */
        static auto shutdown() -> void;

        /**
         * \brief Push a renderable object to the queue
         */
        auto submit(Renderable renderable) -> void;

        /**
         * \brief Explicitly flush the queue
         */
        auto flushBatch() -> void;

        /**
         * \brief Get render stats for the previous frame
         */
        auto getStats() -> RendererStats;

        /**
         * \brief Zero out all stats counters
         */
        auto clearStats() -> void;

        /**
         * \brief Set the view and projection matrices
         */
        auto updateVp(glm::mat4 view, glm::mat4 projection) -> void;

        /**
         * \brief Request a shader program be constructed
         *
         * Renderable objects should request a shader program here. If a matching
         * shader already exists, it will be returned - otherwise it will be constructed
         * and cached.
         */
        auto makeShaderProgram(const Uuid& vertex_shader = {}, const Uuid& fragment_shader = {}) -> ShaderProgram*;

        Renderer();
        ~Renderer();

    private:
        auto flush(unsigned int shader_program_id, glm::mat4 mvp, int mvp_id) -> void;

        std::vector<Renderable> m_renderables{};

        Vertex* m_vertex_buffer;
        Vertex* m_vertex_buffer_ptr;
        GLuint  m_vao;
        GLuint  m_vbo;
        GLuint  m_ibo;

        int                                   m_index_count{0};
        std::array<uint32_t, max_index_count> m_indices;

        uint32_t                           m_texture_count{0};
        std::array<uint32_t, max_textures> m_textures;

        uint32_t m_empty_tex_id{0};

        // statistics per call
        int m_quad_draws{0};
        int m_draw_calls{0};
        int m_texture_binds{0};
        int m_shader_changes{0};

        glm::vec4 m_camera_pos{0};

        glm::mat4 m_view_matrix;
        glm::mat4 m_projection_matrix;

        std::vector<std::unique_ptr<ShaderProgram>> m_shaders{};

        static std::unique_ptr<Renderer> s_instance;
    };

}// namespace rosa