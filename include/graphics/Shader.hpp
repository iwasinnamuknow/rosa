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

#include <graphics/gl.h>
#include <GLFW/glfw3.h>
#include <spdlog/spdlog.h>
#include <string_view>
#include <stduuid/uuid.h>
#include <spdlog/logger.h>

namespace rosa {

    enum ShaderType {
        VertexShader = GL_VERTEX_SHADER,
        FragmentShader = GL_FRAGMENT_SHADER
    };

    constexpr auto default_vertex_shader = std::string_view(R"(
#version 330 core

layout (location = 0) in vec2 inPosition;
layout (location = 1) in vec2 inUV;
layout (location = 2) in vec4 inColor;

uniform mat4 mvp;

out vec4 passColor;
out vec2 UV;

void main()
{
    gl_Position = mvp * vec4(inPosition, 0.0, 1.0);
    passColor = inColor;
    UV = inUV;
}
)");

    constexpr auto default_fragment_shader = std::string_view(R"(
#version 330 core

in vec4 passColor;
in vec2 UV;

out vec4 color;

uniform sampler2D textureSampler;

void main()
{
    color = texture2D(textureSampler, UV).rgba * passColor;
}
)");

    class Shader {
        public:
            explicit Shader(ShaderType type = VertexShader) : m_type(type) {
                if (m_type == VertexShader) {
                    m_content = default_vertex_shader;
                } else if (m_type == FragmentShader) {
                    m_content = default_fragment_shader;
                }
            }

            auto getType() -> ShaderType {
                return m_type;
            }

            auto loadFromPhysFS(const std::string& name) -> void;
            auto link(unsigned int program_id) -> void;

        private:
            ShaderType m_type;
            std::string m_content;
            unsigned int m_shader_id;
    };

} // namespace rosa