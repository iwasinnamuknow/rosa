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
 *  You should have received a copy of the GNU General Public License along with rosa. If not,
 *  see <https://www.gnu.org/licenses/>.
 */

#pragma once

#include <cstdint>

namespace rosa {

    struct Colour {

        Colour() = default;
        Colour(float red, float green, float blue) : r(red), g(green), b(blue) {}
        Colour(float red, float green, float blue, float alpha) : r(red), g(green), b(blue), a(alpha) {}

        float r{1};
        float g{1};
        float b{1};
        float a{1};

    };

} // namespace rosa
