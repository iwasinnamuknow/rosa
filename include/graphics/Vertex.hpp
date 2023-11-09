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

#pragma once

#include <cstdint>
#include <graphics/Colour.hpp>
#include <glm/glm.hpp>

namespace rosa {

    struct Vertex {

        Vertex() = default;
        Vertex(glm::vec2 pos) : position(pos) {}
        Vertex(glm::vec2 pos, Colour col) : position(pos), colour(col) {}
        Vertex(glm::vec2 pos, Colour col, glm::vec2 uv) : position(pos), texture_coords(uv), colour(col) {}
        Vertex(float x, float y) : position(glm::vec2(x, y)) {}
        Vertex(float x, float y, Colour col) : position(glm::vec2(x, y)), colour(col) {}

        glm::vec2 position{0, 0};
        glm::vec2 texture_coords{0, 0};
        Colour colour{255, 255, 255, 255};
        float texture_slot{0.F};
    };

} // namespace rosa