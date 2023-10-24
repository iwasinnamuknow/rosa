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

    auto Shader::link(unsigned int program_id) -> void {
        m_shader_id = glCreateShader(m_type);

        GLint result = GL_FALSE;
        int info_log_length{0};

        // Compile shader
        spdlog::debug("Compiling shader");
        const char* vertex_source_ptr = m_content.c_str();
        glShaderSource(m_shader_id, 1, &vertex_source_ptr , nullptr);
        glCompileShader(m_shader_id);

        // Check shader
        glGetShaderiv(m_shader_id, GL_COMPILE_STATUS, &result);
        glGetShaderiv(m_shader_id, GL_INFO_LOG_LENGTH, &info_log_length);
        if ( info_log_length > 0 ){
            std::vector<char> error(info_log_length + 1);
            glGetShaderInfoLog(m_shader_id, info_log_length, nullptr, error.data());
            spdlog::error("Failed to compile shader: {}", error.data());
        }

        // Link shader to program
        spdlog::debug("Linking shader");
        glAttachShader(program_id, m_shader_id);
        glLinkProgram(program_id);

        // Check the program
        glGetProgramiv(program_id, GL_LINK_STATUS, &result);
        glGetProgramiv(program_id, GL_INFO_LOG_LENGTH, &info_log_length);
        if ( info_log_length > 0 ){
            std::vector<char> error(info_log_length + 1);
            glGetProgramInfoLog(program_id, info_log_length, NULL, error.data());
            spdlog::error("Failed to link shaders: {}", error.data());
        }

        glDetachShader(program_id, m_shader_id);
        glDeleteShader(m_shader_id);
    }

} // namespace rosa