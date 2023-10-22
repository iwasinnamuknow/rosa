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

#include <cstdint>

namespace rosa {

    class ResourceStream {
        public:
            ResourceStream() = default;
            virtual ~ResourceStream() = default;
            virtual auto isOpen() const -> bool = 0;
            virtual auto open(const char * filename) -> bool = 0;
            virtual auto close() -> void = 0;
            virtual auto read(void * data, std::uint64_t size) -> uint64_t = 0;
            virtual auto seek(uint64_t position) -> uint64_t = 0;
            virtual auto tell() -> uint64_t = 0;
            virtual auto getSize() -> uint64_t = 0;
    };

} // namespace rosa