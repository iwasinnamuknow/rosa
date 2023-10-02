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
#include <SFML/System/InputStream.hpp>
#include <SFML/System/NonCopyable.hpp>

struct PHYSFS_File;

namespace rosa {

    class PhysFsStream : public sf::InputStream, sf::NonCopyable
    {
        public:
            explicit PhysFsStream(const char * filename = nullptr);
            ~PhysFsStream() override;
            auto isOpen() const -> bool;
            auto open(const char * filename) ->bool;
            auto close() -> void;
            auto read(void * data, sf::Int64 size) -> sf::Int64 override;
            auto seek(sf::Int64 position) -> sf::Int64 override;
            auto tell() -> sf::Int64 override;
            auto getSize() -> sf::Int64 override;

        private:
            PHYSFS_File * m_file;

    };

} // namespace rosa