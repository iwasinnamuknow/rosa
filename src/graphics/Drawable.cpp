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

namespace rosa {

    auto Drawable::link_shaders() -> void {

        bool defaulted_v_shader{false};
        bool defaulted_f_shader{false};

        if (m_vertex_shader == nullptr){ 
            m_vertex_shader = new Shader(VertexShader);
            defaulted_v_shader = true;
            spdlog::warn("Using default vertex shader");
        }
        if (m_fragment_shader == nullptr) {
            m_fragment_shader = new Shader(FragmentShader);
            defaulted_f_shader = true;
            spdlog::warn("Using default fragment shader");
        }

        auto vshader_id = glCreateShader(VertexShader);
        auto fshader_id = glCreateShader(FragmentShader);

        GLint result = GL_FALSE;
        int info_log_length{0};

        // Compile vertex shader
        spdlog::debug("Compiling vertex shader");
        const char* vertex_source_ptr = m_vertex_shader->getSource().c_str();
        glShaderSource(vshader_id, 1, &vertex_source_ptr , nullptr);
        glCompileShader(vshader_id);

        // Check shader
        glGetShaderiv(vshader_id, GL_COMPILE_STATUS, &result);
        glGetShaderiv(vshader_id, GL_INFO_LOG_LENGTH, &info_log_length);
        if ( info_log_length > 0 ){
            std::vector<char> error(info_log_length + 1);
            glGetShaderInfoLog(vshader_id, info_log_length, nullptr, error.data());
            spdlog::error("Failed to compile shader: {}", error.data());
        }
        
        // Compile fragment shader
        spdlog::debug("Compiling fragment shader");
        const char* frag_source_ptr = m_fragment_shader->getSource().c_str();
        glShaderSource(fshader_id, 1, &frag_source_ptr , nullptr);
        glCompileShader(fshader_id);

        // Check shader
        glGetShaderiv(fshader_id, GL_COMPILE_STATUS, &result);
        glGetShaderiv(fshader_id, GL_INFO_LOG_LENGTH, &info_log_length);
        if ( info_log_length > 0 ){
            std::vector<char> error(info_log_length + 1);
            glGetShaderInfoLog(fshader_id, info_log_length, nullptr, error.data());
            spdlog::error("Failed to compile shader: {}", error.data());
        }

        // Link shader to program
        spdlog::debug("Linking shaders");
        glAttachShader(m_pid, vshader_id);
        glAttachShader(m_pid, fshader_id);
        glLinkProgram(m_pid);

        // Check the program
        glGetProgramiv(m_pid, GL_LINK_STATUS, &result);
        glGetProgramiv(m_pid, GL_INFO_LOG_LENGTH, &info_log_length);
        if ( info_log_length > 0 ){
            std::vector<char> error(info_log_length + 1);
            glGetProgramInfoLog(m_pid, info_log_length, NULL, error.data());
            spdlog::error("Failed to link shaders: {}", error.data());
        }

        glDetachShader(m_pid, vshader_id);
        glDetachShader(m_pid, fshader_id);
        glDeleteShader(vshader_id);
        glDeleteShader(fshader_id);

        if (defaulted_v_shader) {
            delete m_vertex_shader;
        }

        if (defaulted_f_shader) {
            delete m_fragment_shader;
        }
    }

} // namespace rosa