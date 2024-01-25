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

    class ShaderProgram {
    public:
        ShaderProgram() = default;
        ShaderProgram(Uuid vertex_shader, Uuid fragment_shader);

        auto loadVertexShader(Uuid vertex_shader) -> void;
        auto loadFragmentShader(Uuid fragment_shader) -> void;

        auto compile() -> void;

        auto operator<(const ShaderProgram& other) const -> bool {
            return m_program_id < other.getProgramId();
        }

        auto getProgramId() const -> unsigned int {
            return m_program_id;
        }

        auto getMvpId() const -> int {
            return m_mvp_id;
        }

        auto isCompiled() const -> bool {
            return m_compiled;
        }

    private:
        unsigned int m_program_id{0};
        Shader*      m_vertex_shader{nullptr};
        Shader*      m_fragment_shader{nullptr};
        int          m_mvp_id{-1};

        bool m_compiled{false};
    };

}// namespace rosa
