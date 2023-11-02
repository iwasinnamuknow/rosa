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

    struct IndexBuffer {
        IndexBuffer() {
            glGenBuffers(1, &ibo_id);
        }

        auto bind(const std::vector<unsigned int>& index_data) -> void {
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_id);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * index_data.size(), index_data.data(), GL_STATIC_DRAW);
        }

        unsigned int ibo_id;
    };
}