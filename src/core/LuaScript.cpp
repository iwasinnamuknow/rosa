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

#include <core/LuaScript.hpp>

namespace rosa {

    auto LuaScript::loadFromPhysFS() -> bool {

        const auto& name = getName();

        if (PHYSFS_exists(name.c_str()) != 0) {
            PHYSFS_file* myfile = PHYSFS_openRead(name.c_str());
            std::string buffer{};
            buffer.resize(PHYSFS_fileLength(myfile));
            std::int64_t length_read = PHYSFS_readBytes(myfile, buffer.data(), static_cast<std::uint64_t>(PHYSFS_fileLength(myfile)));
            assert(length_read == PHYSFS_fileLength(myfile));
            PHYSFS_close(myfile);

            m_content = buffer;
        } else {
            spdlog::critical("Couldn't load script: {}", static_cast<std::string>(getUUID()));
            return false;
        }

        return true;
    }

    auto LuaScript::getContent() const -> const std::string& {
        return m_content;
    }

} // namespace rosa