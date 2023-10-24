/*
 * This file is part of glad.
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
#include <debug/Profiler.hpp>
#include <cstring>
#include <glm/glm.hpp>
#include <graphics/Sprite.hpp>
#include <graphics/gl.h>
#include <GLFW/glfw3.h>

#include <spdlog/spdlog.h>

namespace rosa {

    auto Sprite::draw(glm::mat4 projection, glm::mat4 transform) -> void {

        if (m_texture == nullptr) {
            return;
        } else {
            auto size = m_texture->getSize();

            m_vertices[0].position = glm::vec2(0, 0);
            m_vertices[1].position = glm::vec2(size.x, 0);
            m_vertices[2].position = glm::vec2(0, size.y);
            m_vertices[3].position = glm::vec2(size.x, size.y);
        }

        m_mvp = projection * transform;
        
        glUseProgram(m_pid);
        glBindVertexArray(m_vertex_array);

        glBindBuffer(GL_ARRAY_BUFFER, m_vertex_buffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(m_vertices), m_vertices, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_TRUE, sizeof(Vertex), (void *)(offsetof(Vertex, texture_coords.x)));
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(2, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Vertex), (void *)(offsetof(Vertex, colour.r)));
        glEnableVertexAttribArray(2);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_index_buffer);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(m_index), m_index, GL_STATIC_DRAW);

        glBindTexture(GL_TEXTURE_2D, m_texture->getOpenGlId());

        glUniformMatrix4fv(m_mvp_id, 1, GL_FALSE, &m_mvp[0][0]);

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glUseProgram(0);
    }

    auto Sprite::gl_init() -> void {

        m_pid = glCreateProgram();
        glUseProgram(m_pid);

        // buffers
        glGenVertexArrays(1, &m_vertex_array);
        glBindVertexArray(m_vertex_array);

        glGenBuffers(1, &m_vertex_buffer);
        glBindBuffer(GL_ARRAY_BUFFER, m_vertex_buffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(m_vertices), m_vertices, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)0);
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 2, GL_FLOAT, GL_TRUE, sizeof(Vertex), (void *)(offsetof(Vertex, texture_coords.x)));
        glEnableVertexAttribArray(1);

        glVertexAttribPointer(2, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Vertex), (void *)(offsetof(Vertex, colour.r)));
        glEnableVertexAttribArray(2);

        glGenBuffers(1, &m_index_buffer);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_index_buffer);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(m_index), m_index, GL_STATIC_DRAW);

        if (m_texture != nullptr) {
            glBindTexture(GL_TEXTURE_2D, m_texture->getOpenGlId());
        }

        m_mvp_id = glGetUniformLocation(m_pid, "mvp");

        m_vertex_shader = new Shader(VertexShader);
        m_vertex_shader->link(m_pid);

        m_fragment_shader = new Shader(FragmentShader);
        m_fragment_shader->link(m_pid);

        glUseProgram(0);

        m_vertices[0].texture_coords = glm::vec2{0, 0};
        m_vertices[1].texture_coords = glm::vec2{1, 0};
        m_vertices[2].texture_coords = glm::vec2{0, 1};
        m_vertices[3].texture_coords = glm::vec2{1, 1};
    }

} // namespace rosa