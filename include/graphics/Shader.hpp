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
     * \brief Handles OpenGL shader compilation and storage
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
         * \param pack Mount point of the asset pack
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
