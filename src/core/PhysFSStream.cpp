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

#include "core/PhysFSStream.hpp"
#include <physfs.h>

namespace rosa {

    PhysFsStream::PhysFsStream(const char * filename) : m_file(nullptr)
    {
        if(filename != nullptr) {
            open(filename);
        }
    }

    PhysFsStream::~PhysFsStream()
    {
        close();
    }

    auto PhysFsStream::isOpen() const -> bool
    {
        return (m_file != nullptr);
    }

    auto PhysFsStream::open(const char * filename) -> bool
    {
        close();
        m_file = PHYSFS_openRead(filename);
        return isOpen();
    }

    auto PhysFsStream::close() -> void
    {
        if(isOpen()) {
            PHYSFS_close(m_file);
        }
        m_file = nullptr;
    }

    auto PhysFsStream::read(void * data, sf::Int64 size) -> sf::Int64
    {
        if(!isOpen()) {
            return -1;
        }

        // PHYSFS_readBytes returns the number of bytes read or -1 on error.
        // We request to read size amount of bytes.
        return PHYSFS_readBytes(m_file, data, static_cast<PHYSFS_uint32>(size));
    }

    auto PhysFsStream::seek(sf::Int64 position) -> sf::Int64
    {
        if(!isOpen()) {
            return -1;
        }

        // PHYSFS_seek return 0 on error and non zero on success
        if(PHYSFS_seek(m_file, position) != 0) {
            return tell();
        }

        return -1;
    }

    auto PhysFsStream::tell() -> sf::Int64
    {
        if(!isOpen()) {
            return -1;
        }

        // PHYSFS_tell returns the offset in bytes or -1 on error just like SFML wants.
        return PHYSFS_tell(m_file);
    }

    auto PhysFsStream::getSize() -> sf::Int64
    {
        if(!isOpen()) {
            return -1;
        }

        // PHYSFS_fileLength also the returns length of file or -1 on error just like SFML wants.
        return PHYSFS_fileLength(m_file);
    }
} // namespace rosa