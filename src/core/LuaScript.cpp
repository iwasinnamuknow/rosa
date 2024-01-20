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

    auto LuaScript::loadFromPhysFS() -> void {

        const auto& name = getName();

        if (PHYSFS_exists(name.c_str()) == 0) {
            throw ResourceNotFoundException(fmt::format("Couldn't find script {}", name));
        }

        PHYSFS_file* file = PHYSFS_openRead(name.c_str());

        std::string buffer{};
        buffer.resize(static_cast<std::uint64_t>(PHYSFS_fileLength(file)));

        std::int64_t length_read = PHYSFS_readBytes(file, buffer.data(), static_cast<std::uint64_t>(PHYSFS_fileLength(file)));
        assert(length_read == PHYSFS_fileLength(file));

        PHYSFS_close(file);
        m_content = buffer;
    }

    auto LuaScript::getContent() const -> const std::string& {
        return m_content;
    }

}// namespace rosa