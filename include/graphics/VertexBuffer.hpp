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

#include <glm/glm.hpp>
#include <graphics/gl.hpp>
#include <GLFW/glfw3.h>

namespace rosa {

    struct VertexAttribPointer {
        unsigned int attribute_id;
        int count;
        unsigned int type;
        unsigned char normalised;
        int size;
        int stride;
    };

    class VertexBuffer {
        public:
            VertexBuffer() {
                glGenVertexArrays(1, &vao_id);
                glGenBuffers(1, &vbo_id);
            }

            auto bind(const std::vector<Vertex>& vertex_data) -> void {
                glBindVertexArray(vao_id);
                glBindBuffer(GL_ARRAY_BUFFER, vbo_id);
                glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertex_data.size(), vertex_data.data(), GL_STATIC_DRAW);
                
                for (const auto& ptr : m_attributes) {
                    glVertexAttribPointer(ptr.attribute_id, ptr.count, ptr.type, ptr.normalised, ptr.size, (void *)(ptr.stride));
                    glEnableVertexAttribArray(ptr.attribute_id);
                }
                
                glVertexAttribPointer(1, 2, GL_FLOAT, GL_TRUE, sizeof(Vertex), (void *)(offsetof(Vertex, texture_coords.x)));
                glEnableVertexAttribArray(1);

                glad_glVertexAttribPointer(2, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Vertex), (void *)(offsetof(Vertex, colour.r)));
                glEnableVertexAttribArray(2);
            }

            auto addAttribute(VertexAttribPointer attrib) -> void {
                m_attributes.push_back(attrib);
            }

        private:

            unsigned int vao_id;
            unsigned int vbo_id;

            std::vector<VertexAttribPointer> m_attributes;
    };
}