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

    Shader::Shader(const std::string& name, Uuid uuid, const std::string& pack, ShaderType type)
        : Resource(name, uuid, pack), m_type(type) {

        if (m_type == VertexShader) {
            m_content = default_vertex_shader;
        } else if (m_type == FragmentShader) {
            m_content = default_fragment_shader;
        }

        if (name == "default") {
            return;
        }

        if (PHYSFS_exists(name.c_str()) == 0) {
            throw ResourceNotFoundException(fmt::format("Couldn't find shader {}", name));
        }

        PHYSFS_file* file = PHYSFS_openRead(name.c_str());

        std::int64_t length_read = PHYSFS_readBytes(file, m_content.data(), static_cast<std::uint64_t>(PHYSFS_fileLength(file)));
        assert(length_read == PHYSFS_fileLength(file));
    }

}// namespace rosa