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

#include <core/Uuid.hpp>
#include <core/Resource.hpp>
#include <soloud/include/soloud_file.h>
#include <physfs.h>

namespace rosa {

    class AudioFile : public ::SoLoud::File, public rosa::Resource {

        public:
            AudioFile(std::string name, Uuid uuid) : Resource(std::move(name), uuid) {}
            ~AudioFile() override;

            auto loadFromPhysFS() -> bool override;

            auto eof() -> int override;
            auto read(unsigned char* dest, unsigned int bytes) -> unsigned int override;
            auto length() -> unsigned int override;
            auto seek(int offset) -> void override;
            auto pos() -> unsigned int override;

        private:
            PHYSFS_File* m_file_ptr{nullptr};
    };

} // namespace rosa