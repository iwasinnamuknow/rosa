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

#include <core/Uuid.hpp>
#include <core/Resource.hpp>
#include <soloud/include/soloud_file.h>
#include <physfs.h>

namespace rosa {

    /**
     * \brief Basic audio storage
     *
     *  Allows us to load audio data from the resource manager, play it
     *  back and seek around etc. This class shouldn't be used directly
     *  and is handled by the audio components.
     */
    class AudioFile : public ::SoLoud::File, public rosa::Resource {

    public:
        /**
         * \brief Construct an audio file object
         * \param name filename of the asset in the pack
         * \param uuid asset uuid
         * \param pack additional pack path for submounts
         */
        AudioFile(std::string name, Uuid uuid, std::string pack) : Resource(std::move(name), uuid, std::move(pack)) {}

        ~AudioFile() override;

        /**
         * \brief Load audio data from asset pack
         * \return loading success or failure
         */
        auto loadFromPhysFS() -> bool override;

        /**
         * \brief Check if the audio read head is at the end of the file
         * \return non-zero if EOF, zero if not
         */
        auto eof() -> int override;

        /**
         * \brief Read some bytes from the asset source
         * \param dest destination pointer
         * \param bytes number of bytes to read
         * \return number of bytes read
         */
        auto read(unsigned char *dest, unsigned int bytes) -> unsigned int override;

        /**
         * \brief Get the size of the file object
         * \return size in bytes
         */
        auto length() -> unsigned int override;

        /**
         * \brief Move the read head
         * \param offset position in bytes
         */
        auto seek(int offset) -> void override;

        /**
         * \brief Get the current read head position
         * \return position in bytes
         */
        auto pos() -> unsigned int override;

        /**
         * \brief Explicit destruction of backing files
         */
        auto closeHandles() -> void override;

    private:
        PHYSFS_File *m_file_ptr{nullptr};
    };

} // namespace rosa