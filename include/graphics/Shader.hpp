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

/*! \file */

#pragma once

#include <GLFW/glfw3.h>
#include <core/Resource.hpp>
#include <core/Uuid.hpp>
#include <graphics/gl.hpp>
#include <spdlog/spdlog.h>
#include <string_view>

namespace rosa {

    /**
     * \brief Differentiates between OpenGL shader types
     *
     * In essence shaders are just strings of text until they are compiled
     * so this enum is the only thing to differentiate shaders.
     */
    enum ShaderType {
        VertexShader   = GL_VERTEX_SHADER,
        FragmentShader = GL_FRAGMENT_SHADER
    };

    /**
     * \brief The default vertex shader used if no other shader is provided
     */
    constexpr auto default_vertex_shader = std::string_view(R"(
#version 420 core

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

    /**
     * \brief The default fragment shader if no other shader is provided
     */
    constexpr auto default_fragment_shader = std::string_view(R"(
#version 420 core

in vec4 passColor;
in vec2 UV;
in float texture;

out vec4 color;

layout(binding=0) uniform sampler2D textureSamplers[32];

void main()
{
    int index = int(texture);
    color = texture2D(textureSamplers[index], UV).rgba * passColor;
}
)");

    /**
     * \brief Used to handle OpenGL shader compilation and storage
     *
     * When constructed the shader will be retrieved from disk and stored in a string.
     * This string can be provided to the Renderer and compiled/uploaded to the GPU
     */
    class Shader : public ::rosa::Resource {
    public:
        /**
         * \brief Construct a shader object
         * \param name Filename relative to it's asset pack
         * \param uuid Uuid to associate with the shader
         * \param pack Mountpoint of the asset pack
         * \param type ShaderType defaulting to VertexShader
         */
        explicit Shader(const std::string& name, const Uuid& uuid, const std::string& pack, ShaderType type = VertexShader);

        /**
         * \brief Get the ShaderType of the asset
         */
        auto getType() const -> ShaderType {
            return m_type;
        }

        /**
         * \brief Get the source of the shader as text
         */
        auto getSource() const -> const std::string& {
            return m_content;
        }

    private:
        ShaderType  m_type;
        std::string m_content;
    };

}// namespace rosa
