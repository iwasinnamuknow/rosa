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

#pragma once

#include "graphics/RenderWindow.hpp"
#include <graphics/Vertex.hpp>
#include <graphics/VertexBuffer.hpp>
#include <graphics/IndexBuffer.hpp>
#include <glm/glm.hpp>

namespace rosa {

    class Drawable {
        public:
            friend class RenderWindow;
            Drawable() {
                m_pid  = glCreateProgram();
                link_shaders();
                m_mvp_id = glGetUniformLocation(m_pid, "mvp");
            }
            virtual ~Drawable() = default;
            virtual auto draw(glm::mat4 projection, glm::mat4 transform) -> void{}

        protected:
            friend class RenderWindow;

            std::vector<Vertex> m_vertices;
            VertexBuffer m_vbo;

            std::vector<unsigned int> m_indices;
            IndexBuffer m_ibo;

            unsigned int m_pid;

            auto link_shaders() -> void;

            Shader* m_vertex_shader{nullptr};
            Shader* m_fragment_shader{nullptr};

            glm::mat4 m_mvp{1.F};
            int m_mvp_id;
    };

} // namespace rosa