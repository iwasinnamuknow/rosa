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

#include <core/Uuid.hpp>
#include <graphics/Shader.hpp>

namespace rosa {

    /**
     * \brief Manages an OpenGL shader program
     *
     * Shader programs are made up of a vertex shader plus a fragment shader.
     * If one of those shaders is not provided or an empty Uuid is provided, a
     * default hardcoded shader will be used.
     *
     * On compilation, the shaders will be sent to the GPU and linked into a program
     * which can then be used when rendering.
     */
    class ShaderProgram {
    public:
        /**
         * \brief Create a shader program with default shaders
         */
        ShaderProgram() = default;

        /**
         * \brief Create a shader program with provided shaders
         */
        ShaderProgram(const Uuid& vertex_shader, const Uuid& fragment_shader);

        /**
         * \brief Set the vertex shader, this will require re-compilation
         */
        auto loadVertexShader(const Uuid& vertex_shader) -> void;

        /**
         * \brief Set the fragment shader, this will require re-compilation
         */
        auto loadFragmentShader(const Uuid& fragment_shader) -> void;

        /**
         * \brief Compile and link the program
         */
        auto compile() -> void;

        /**
         * \brief Comparison operator so we can sort by ID
         */
        auto operator<(const ShaderProgram& other) const -> bool {
            return m_program_id < other.getProgramId();
        }

        /**
         * \brief Get the OpenGL program ID
         *
         * This will be non-zero on compilation
         */
        auto getProgramId() const -> unsigned int {
            return m_program_id;
        }

        /**
         * \brief Get the uniform ID for the MVP
         */
        auto getMvpId() const -> int {
            return m_mvp_id;
        }

        /**
         * \brief Check if the program has been compiled
         */
        auto isCompiled() const -> bool {
            return m_compiled;
        }

        /**
         * \brief Get the Uuid of the vertex shader
         */
        auto getVertexShaderUuid() const -> const Uuid& {
            return m_vertex_shader_id;
        }

        /**
         * \brief Get the Uuid of the fragment shader
         */
        auto getFragmentShaderUuid() const -> const Uuid& {
            return m_fragment_shader_id;
        }

    private:
        unsigned int m_program_id{0};
        Shader*      m_vertex_shader{nullptr};
        Shader*      m_fragment_shader{nullptr};
        int          m_mvp_id{-1};
        Uuid         m_vertex_shader_id{};
        Uuid         m_fragment_shader_id{};
        bool         m_compiled{false};
    };

}// namespace rosa
