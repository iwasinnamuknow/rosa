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

#include <core/ResourceStream.hpp>
#include <cstdint>

struct PHYSFS_File;

namespace rosa {

    class PhysFsStream : public ResourceStream
    {
        public:
            explicit PhysFsStream(const char * filename = nullptr);
            ~PhysFsStream() override;
            auto isOpen() const -> bool override;
            auto open(const char * filename) -> bool override;
            auto close() -> void override;
            auto read(void * data, uint64_t size) -> uint64_t override;
            auto seek(uint64_t position) -> uint64_t override;
            auto tell() -> uint64_t override;
            auto getSize() -> uint64_t override;

        private:
            PHYSFS_File * m_file;

    };

} // namespace rosa