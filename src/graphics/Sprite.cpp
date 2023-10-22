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

#include <glm/glm.hpp>
#include <graphics/Sprite.hpp>
#include <graphics/gl.h>
#include <GLFW/glfw3.h>

#include <spdlog/spdlog.h>

namespace rosa {

    auto Sprite::draw(glm::mat4 projection) -> void {

        if (m_texture == nullptr) {
            return;
        }

        update_transform(projection);
        
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

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glUseProgram(0);
    }

    auto Sprite::update_transform(glm::mat4 projection) -> void {

        //if (transform_dirty) {
            m_mvp = projection * getLocalTransform();
            glm::vec4 transformed = m_mvp * glm::vec4(1.F, 1.F, 1.F, 1.F);

            if (m_texture != nullptr) {
                auto size = m_texture->getSize();

                m_vertices[0].position = glm::vec2(transformed[0], transformed[1]);
                m_vertices[1].position = glm::vec2(transformed[0] + size.x, transformed[1]);
                m_vertices[2].position = glm::vec2(transformed[0], transformed[1] + size.y);
                m_vertices[3].position = glm::vec2(transformed[0] + size.x, transformed[1] + size.y);
            }

        //     transform_dirty = false;
        // }
    }

    auto Sprite::getLocalTransform() const -> const glm::mat4 {
        glm::mat4 translation_matrix = glm::translate(glm::mat4(1.F), m_position);
        glm::mat4 scale_matrix = glm::scale(glm::mat4(1.F), m_scale);
        return translation_matrix * scale_matrix;
    }

    auto Sprite::gl_init() -> void {

        m_pid = load_shaders("shader_basic.vert", "shader_basic.frag");
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

        if (m_texture != nullptr) {
            glBindTexture(GL_TEXTURE_2D, m_texture->getID());
        }

        m_mvp_id = glGetUniformLocation(m_pid, "mvp");

        glUseProgram(0);

        m_vertices[0].texture_coords = glm::vec2{0, 0};
        m_vertices[1].texture_coords = glm::vec2{1, 0};
        m_vertices[2].texture_coords = glm::vec2{0, 1};
        m_vertices[3].texture_coords = glm::vec2{1, 1};
    }

    auto Sprite::load_shaders(const char * vertex_file_path,const char * fragment_file_path) -> unsigned int {
                
        // Create the shaders
        GLuint vertex_shader_id = glCreateShader(GL_VERTEX_SHADER);
        GLuint fragment_shader_id = glCreateShader(GL_FRAGMENT_SHADER);

        // Read the Vertex Shader code from the file
        std::string vertex_shader_source;
        std::ifstream vertex_shader_stream(vertex_file_path, std::ios::in);
        if(vertex_shader_stream.is_open()){
            std::stringstream sstr;
            sstr << vertex_shader_stream.rdbuf();
            vertex_shader_source = sstr.str();
            vertex_shader_stream.close();
        }else{
            spdlog::error("Couldn't open vertex shader: {}", vertex_file_path);
            return 0;
        }

        // Read the Fragment Shader code from the file
        std::string fragment_shader_source;
        std::ifstream fragment_shader_stream(fragment_file_path, std::ios::in);
        if(fragment_shader_stream.is_open()){
            std::stringstream sstr;
            sstr << fragment_shader_stream.rdbuf();
            fragment_shader_source = sstr.str();
            fragment_shader_stream.close();
        } else {
            spdlog::error("Couldn't open fragment shader: {}", fragment_file_path);
            return 0;
        }

        GLint result = GL_FALSE;
        int info_log_length{0};

        // Compile Vertex Shader
        spdlog::debug("Compiling shader {}", vertex_file_path);
        const char* vertex_source_ptr = vertex_shader_source.c_str();
        glShaderSource(vertex_shader_id, 1, &vertex_source_ptr , nullptr);
        glCompileShader(vertex_shader_id);

        // Check Vertex Shader
        glGetShaderiv(vertex_shader_id, GL_COMPILE_STATUS, &result);
        glGetShaderiv(vertex_shader_id, GL_INFO_LOG_LENGTH, &info_log_length);
        if ( info_log_length > 0 ){
            std::vector<char> error(info_log_length + 1);
            glGetShaderInfoLog(vertex_shader_id, info_log_length, nullptr, error.data());
            spdlog::error("Failed to compile shader: {}", error.data());
        }

        // Compile Fragment Shader
        spdlog::debug("Compiling shader {}", fragment_file_path);
        char const * fragment_source_ptr = fragment_shader_source.c_str();
        glShaderSource(fragment_shader_id, 1, &fragment_source_ptr , nullptr);
        glCompileShader(fragment_shader_id);

        // Check Fragment Shader
        glGetShaderiv(fragment_shader_id, GL_COMPILE_STATUS, &result);
        glGetShaderiv(fragment_shader_id, GL_INFO_LOG_LENGTH, &info_log_length);
        if ( info_log_length > 0 ){
            std::vector<char> error(info_log_length+1);
            glGetShaderInfoLog(fragment_shader_id, info_log_length, nullptr, error.data());
            spdlog::error("Failed to compile shader: {}", error.data());
        }

        // Link the program
        spdlog::debug("Linking shaders");
        GLuint pid = glCreateProgram();
        glAttachShader(pid, vertex_shader_id);
        glAttachShader(pid, fragment_shader_id);
        glLinkProgram(pid);

        // Check the program
        glGetProgramiv(pid, GL_LINK_STATUS, &result);
        glGetProgramiv(pid, GL_INFO_LOG_LENGTH, &info_log_length);
        if ( info_log_length > 0 ){
            std::vector<char> error(info_log_length + 1);
            glGetProgramInfoLog(pid, info_log_length, NULL, error.data());
            spdlog::error("Failed to link shaders: {}", error.data());
        }
        
        glDetachShader(pid, vertex_shader_id);
        glDetachShader(pid, fragment_shader_id);
        
        glDeleteShader(vertex_shader_id);
        glDeleteShader(fragment_shader_id);

        return pid;
    }

} // namespace rosa