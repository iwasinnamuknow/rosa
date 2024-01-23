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

#include <audio/AudioFile.hpp>
#include <string>

namespace rosa {

    AudioFile::AudioFile(const std::string& name, Uuid uuid, const std::string& pack)
        : Resource(name, uuid, pack) {

        if (PHYSFS_exists(name.c_str()) == 0) {
            throw ResourceNotFoundException(fmt::format("Couldn't find audio file {}", name));
        }

        m_file_ptr = PHYSFS_openRead(name.c_str());
    }

    AudioFile::~AudioFile() {
        PHYSFS_close(m_file_ptr);
    }

    auto AudioFile::eof() -> int {
        return PHYSFS_eof(m_file_ptr);
    }

    auto AudioFile::read(unsigned char* dest, unsigned int bytes) -> unsigned int {
        return PHYSFS_readBytes(m_file_ptr, dest, bytes);
    }

    auto AudioFile::length() -> unsigned int {
        return PHYSFS_fileLength(m_file_ptr);
    }

    auto AudioFile::seek(int offset) -> void {
        PHYSFS_seek(m_file_ptr, static_cast<std::uint64_t>(offset));
    }

    auto AudioFile::pos() -> unsigned int {
        return PHYSFS_tell(m_file_ptr);
    }

    auto AudioFile::closeHandles() -> void {
        PHYSFS_close(m_file_ptr);
    }

}// namespace rosa