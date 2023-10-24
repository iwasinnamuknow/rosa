/*
 * This file is part of glad.
 *
 *  glad is free software: you can redistribute it and/or modify it under the terms of the
 *  GNU General Public License as published by the Free Software Foundation, either version
 *  3 of the License, or (at your option) any later version.
 *
 *  glad is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without
 *  even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along with bbai. If not,
 *  see <https://www.gnu.org/licenses/>.
 */

#include "graphics/Shader.hpp"
#include <debug/Profiler.hpp>
#include <cstring>
#include <glm/glm.hpp>
#include <graphics/Sprite.hpp>
#include <graphics/gl.h>
#include <GLFW/glfw3.h>

#include <spdlog/spdlog.h>

namespace rosa {

    auto Sprite::draw(glm::mat4 projection, glm::mat4 transform) -> void {

        m_mvp = projection * transform;

        if (m_texture == nullptr) {
            return;
        } else {
            auto size = m_texture->getSize();

            m_vertices[0].position = glm::vec2(0, 0);
            m_vertices[1].position = glm::vec2(size.x, 0);
            m_vertices[2].position = glm::vec2(0, size.y);
            m_vertices[3].position = glm::vec2(size.x, size.y);
        }
        
        glUseProgram(m_pid);
        glBindVertexArray(m_vertex_array);

        glBindBuffer(GL_ARRAY_BUFFER, m_vertex_buffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(m_vertices), m_vertices, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_TRUE, sizeof(Vertex), (void *)(offsetof(Vertex, texture_coords.x)));
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(2, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Vertex), (void *)(offsetof(Vertex, colour.r)));
        glEnableVertexAttribArray(2);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_index_buffer);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(m_index), m_index, GL_STATIC_DRAW);
        glUniformMatrix4fv(m_mvp_id, 1, GL_FALSE, &m_mvp[0][0]);

        glBindTexture(GL_TEXTURE_2D, m_texture->getOpenGlId());

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glUseProgram(0);
    }

    auto Sprite::gl_init() -> void {

        m_pid = glCreateProgram();
        m_vertex_shader = new Shader(VertexShader);
        m_fragment_shader = new Shader(FragmentShader);
        link_shaders();

        glUseProgram(m_pid);

        // buffers
        glGenVertexArrays(1, &m_vertex_array);
        glBindVertexArray(m_vertex_array);

        glGenBuffers(1, &m_vertex_buffer);
        glBindBuffer(GL_ARRAY_BUFFER, m_vertex_buffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(m_vertices), m_vertices, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)0);
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 2, GL_FLOAT, GL_TRUE, sizeof(Vertex), (void *)(offsetof(Vertex, texture_coords.x)));
        glEnableVertexAttribArray(1);

        glVertexAttribPointer(2, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Vertex), (void *)(offsetof(Vertex, colour.r)));
        glEnableVertexAttribArray(2);

        glGenBuffers(1, &m_index_buffer);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_index_buffer);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(m_index), m_index, GL_STATIC_DRAW);

        m_mvp_id = glGetUniformLocation(m_pid, "mvp");

        if (m_texture != nullptr) {
            glBindTexture(GL_TEXTURE_2D, m_texture->getOpenGlId());
        }

        glUseProgram(0);

        m_vertices[0].texture_coords = glm::vec2{0, 0};
        m_vertices[1].texture_coords = glm::vec2{1, 0};
        m_vertices[2].texture_coords = glm::vec2{0, 1};
        m_vertices[3].texture_coords = glm::vec2{1, 1};
    }

    auto Sprite::link_shaders() -> void {
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
    }

} // namespace rosa