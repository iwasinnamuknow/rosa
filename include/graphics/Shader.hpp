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

#pragma once

#include <graphics/gl.hpp>
#include <GLFW/glfw3.h>
#include <spdlog/spdlog.h>
#include <string_view>
#include <core/Uuid.hpp>
#include <core/Resource.hpp>

namespace rosa {

    enum ShaderType {
        VertexShader = GL_VERTEX_SHADER,
        FragmentShader = GL_FRAGMENT_SHADER
    };

    constexpr auto default_vertex_shader = std::string_view(R"(
#version 400 core

layout (location = 0) in vec2 inPosition;
layout (location = 1) in vec2 inUV;
layout (location = 2) in vec4 inColor;
layout (location = 3) in float inTexture;

uniform mat4 mvp;

out vec4 passColor;
out vec2 UV;
out float texture;

void main()
{
    gl_Position = mvp * vec4(inPosition, 0.0, 1.0);
    passColor = inColor;
    UV = inUV;
    texture = inTexture;
}
)");

    constexpr auto default_fragment_shader = std::string_view(R"(
#version 400 core

in vec4 passColor;
in vec2 UV;
in float texture;

out vec4 color;

uniform sampler2D textureSamplers[32];

void main()
{
    int index = int(texture);
    color = texture2D(textureSamplers[index], UV).rgba * passColor;
}
)");

    class Shader : public ::rosa::Resource {
        public:
            explicit Shader(const std::string& name, Uuid uuid, std::string pack, ShaderType type = VertexShader) : Resource(std::move(name), uuid, std::move(pack)), m_type(type) {
                if (m_type == VertexShader) {
                    m_content = default_vertex_shader;
                } else if (m_type == FragmentShader) {
                    m_content = default_fragment_shader;
                }
            }

            auto getType() const -> ShaderType {
                return m_type;
            }

            auto loadFromPhysFS() -> bool override;

            auto getSource() const -> const std::string& {
                return m_content;
            }

        private:
            ShaderType m_type;
            std::string m_content;
            unsigned int m_shader_id;
    };

} // namespace rosa
