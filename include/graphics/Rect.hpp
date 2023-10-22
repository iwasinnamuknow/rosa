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

#pragma once

namespace rosa {

    template <typename T>
    struct Rect {

        Rect() = default;
        Rect(T x, T y, T w, T h) : left(x), top(y), width(w), height(h) {}

        T left{0};
        T top{0};
        T width{0};
        T height{0};

    };
}