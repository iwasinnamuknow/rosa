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

namespace rosa {

    struct Colour {

        Colour() = default;
        Colour(std::uint8_t red, std::uint8_t green, std::uint8_t blue) : r(red), g(green), b(blue) {}
        Colour(std::uint8_t red, std::uint8_t green, std::uint8_t blue, std::uint8_t alpha) : r(red), g(green), b(blue), a(alpha) {}

        std::uint8_t r{255};
        std::uint8_t g{255};
        std::uint8_t b{255};
        std::uint8_t a{255};

    };

} // namespace rosa