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
#include <graphics/Colour.hpp>

namespace rosa {

    struct Quad {
        glm::vec2 size{0.F, 0.F};
        glm::vec2 pos{0.F, 0.F};
        Colour colour;
        uint32_t texture_id{0};
        glm::vec2 texture_rect_pos{0, 0};
        glm::vec2 texture_rect_size{0, 0};
    };

} // namespace rosa