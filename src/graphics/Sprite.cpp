/*
 * This file is part of rosa.
 *
 *  glad is free software: you can redistribute it and/or modify it under the terms of the
 *  GNU General Public License as published by the Free Software Foundation, either version
 *  3 of the License, or (at your option) any later version.
 *
 *  glad is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without
 *  even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along with bbai. If not,
 *  see <https://www.gnu.org/licenses/>.
 */

#include "graphics/Shader.hpp"
#include "graphics/Vertex.hpp"
#include "graphics/VertexBuffer.hpp"
#include <cstddef>
#include <debug/Profiler.hpp>
#include <cstring>
#include <glm/glm.hpp>
#include <graphics/Sprite.hpp>
#include <graphics/gl.hpp>
#include <GLFW/glfw3.h>

#include <spdlog/spdlog.h>

namespace rosa {

    Sprite::Sprite() {
        m_vertices.resize(4);

        m_vbo.addAttribute({0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0});
        m_vbo.addAttribute({1, 2, GL_FLOAT, GL_TRUE, sizeof(Vertex), offsetof(Vertex, texture_coords.x)});
        m_vbo.addAttribute({2, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Vertex), offsetof(Vertex, colour.r)});
        m_vbo.bind(m_vertices);

        m_indices.insert(m_indices.end(), {
            0, 1, 2,
            1, 2, 3
        });

        m_vertices[0].texture_coords = glm::vec2{0, 0};
        m_vertices[1].texture_coords = glm::vec2{1, 0};
        m_vertices[2].texture_coords = glm::vec2{0, 1};
        m_vertices[3].texture_coords = glm::vec2{1, 1};

        m_vertices.resize(4);
        m_indices.resize(6);
    }

    auto Sprite::draw(glm::mat4 projection, glm::mat4 transform) -> void {

        if (m_texture != nullptr) {
            glBindTexture(GL_TEXTURE_2D, m_texture->getOpenGlId());
        }

        if (m_vertices.size() == 4) {
            auto size = m_texture->getSize();

            m_vertices[0].position = glm::vec2(-(size.x/2), -(size.y/2));
            m_vertices[1].position = glm::vec2(  size.x/2,  -(size.y/2));
            m_vertices[2].position = glm::vec2(-(size.x/2),   size.y/2);
            m_vertices[3].position = glm::vec2(  size.x/2,    size.y/2);
        }
        
        m_vbo.bind(m_vertices);
        m_ibo.bind(m_indices);

        glDrawElements(GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_INT, nullptr);
    }

} // namespace rosa