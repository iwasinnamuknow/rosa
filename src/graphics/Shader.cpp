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

#include <graphics/Shader.hpp>
#include <physfs.h>

namespace rosa {

    Shader::Shader(const std::string& name, const Uuid& uuid, const std::string& pack, ShaderType type)
        : Resource(name, uuid, pack), m_type(type) {

        if (PHYSFS_exists(name.c_str()) == 0) {
            throw ResourceNotFoundException(fmt::format("Couldn't find shader {}", name));
        }

        PHYSFS_file* file = PHYSFS_openRead(name.c_str());
        if (file == nullptr) {
            throw Exception(fmt::format("Failed to open file for reading {}", name));
        }

        PHYSFS_sint64 file_length = PHYSFS_fileLength(file);
        m_content.resize(file_length);

        auto          unsigned_length = static_cast<std::uint64_t>(file_length);
        PHYSFS_sint64 length_read     = PHYSFS_readBytes(file, m_content.data(), unsigned_length);
        if (length_read != file_length) {
            throw Exception(fmt::format("Error reading file {}", name));
        }

        assert(length_read == file_length);
    }

}// namespace rosa