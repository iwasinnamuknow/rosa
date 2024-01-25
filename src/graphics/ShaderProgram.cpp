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


#include <core/ResourceManager.hpp>
#include <graphics/ShaderProgram.hpp>

namespace rosa {
    ShaderProgram::ShaderProgram(Uuid vertex_shader, Uuid fragment_shader)
        : m_vertex_shader(&ResourceManager::getInstance().getAsset<Shader>(vertex_shader)),
          m_fragment_shader(&ResourceManager::getInstance().getAsset<Shader>(fragment_shader)) {
    }

    auto ShaderProgram::loadVertexShader(Uuid vertex_shader) -> void {
        m_vertex_shader = &ResourceManager::getInstance().getAsset<Shader>(vertex_shader);
        m_compiled      = false;
    }

    auto ShaderProgram::loadFragmentShader(Uuid fragment_shader) -> void {
        m_fragment_shader = &ResourceManager::getInstance().getAsset<Shader>(fragment_shader);
        m_compiled        = false;
    }

    auto ShaderProgram::compile() -> void {

        m_program_id = glCreateProgram();

        if (m_vertex_shader == nullptr) {
            m_vertex_shader = new Shader("default", Uuid(), "", VertexShader);
        }

        if (m_fragment_shader == nullptr) {
            m_fragment_shader = new Shader("default", Uuid(), "", FragmentShader);
        }

        auto v_shader_id = glCreateShader(VertexShader);
        auto f_shader_id = glCreateShader(FragmentShader);

        GLint result = GL_FALSE;
        int   info_log_length{0};

        // Compile vertex shader
        spdlog::debug("Compiling vertex shader");
        const char* vertex_source_ptr = m_vertex_shader->getSource().c_str();
        glShaderSource(v_shader_id, 1, &vertex_source_ptr, nullptr);
        glCompileShader(v_shader_id);

        // Check shader
        glGetShaderiv(v_shader_id, GL_COMPILE_STATUS, &result);
        glGetShaderiv(v_shader_id, GL_INFO_LOG_LENGTH, &info_log_length);
        if (info_log_length > 0) {
            std::vector<char> error(static_cast<std::uint64_t>(info_log_length) + 1);
            glGetShaderInfoLog(v_shader_id, info_log_length, nullptr, error.data());
            throw Exception(fmt::format("Failed to compile shader: {}", error.data()));
        }

        // Compile fragment shader
        spdlog::debug("Compiling fragment shader");
        const char* frag_source_ptr = m_fragment_shader->getSource().c_str();
        glShaderSource(f_shader_id, 1, &frag_source_ptr, nullptr);
        glCompileShader(f_shader_id);

        // Check shader
        glGetShaderiv(f_shader_id, GL_COMPILE_STATUS, &result);
        glGetShaderiv(f_shader_id, GL_INFO_LOG_LENGTH, &info_log_length);
        if (info_log_length > 0) {
            std::vector<char> error(static_cast<std::uint64_t>(info_log_length) + 1);
            glGetShaderInfoLog(f_shader_id, info_log_length, nullptr, error.data());
            throw Exception(fmt::format("Failed to compile shader: {}", error.data()));
        }

        // Link shader to program
        spdlog::debug("Linking shaders");
        glAttachShader(m_program_id, v_shader_id);
        glAttachShader(m_program_id, f_shader_id);
        glLinkProgram(m_program_id);

        // Check the program
        glGetProgramiv(m_program_id, GL_LINK_STATUS, &result);
        glGetProgramiv(m_program_id, GL_INFO_LOG_LENGTH, &info_log_length);
        if (info_log_length > 0) {
            std::vector<char> error(static_cast<std::uint64_t>(info_log_length) + 1);
            glGetProgramInfoLog(m_program_id, info_log_length, nullptr, error.data());
            throw Exception(fmt::format("Failed to link shaders: {}", error.data()));
        }

        m_mvp_id = glGetUniformLocation(m_program_id, "mvp");

        glDetachShader(m_program_id, v_shader_id);
        glDetachShader(m_program_id, f_shader_id);
        glDeleteShader(v_shader_id);
        glDeleteShader(f_shader_id);

        m_compiled = true;
    }

}// namespace rosa