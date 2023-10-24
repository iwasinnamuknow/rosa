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

#include <graphics/Shader.hpp>
#include <physfs.h>

namespace rosa {

    auto Shader::loadFromPhysFS(const std::string& name) -> void {

        if (PHYSFS_exists(name.c_str()) == 0) {
            spdlog::error("Couldn't load asset: {}", name);
            return;
        }

        PHYSFS_file* myfile = PHYSFS_openRead(name.c_str());

        int length_read = PHYSFS_readBytes(myfile, m_content.data(), PHYSFS_fileLength(myfile));
        assert(length_read == PHYSFS_fileLength(myfile));
    }

} // namespace rosa