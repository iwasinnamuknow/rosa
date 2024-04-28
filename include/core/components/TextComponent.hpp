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

#include <core/ResourceManager.hpp>
#include <core/Uuid.hpp>
#include <glm/common.hpp>
#include <glm/glm.hpp>
#include <graphics/BitmapFont.hpp>
#include <graphics/Quad.hpp>
#include <graphics/Renderer.hpp>
#include <graphics/ShaderProgram.hpp>

#include <string_view>
#include <vector>

namespace rosa {

    class SceneSerialiser;

    struct TextComponent {

        TextComponent()
            : m_shader_program(Renderer::getInstance().makeShaderProgram(m_vertex_shader, m_fragment_shader)) {}

        /**
         * \brief Set the text to be displayed
         */
        auto setText(const std::string& text) -> void;

        /**
         * \brief Retrieve a previously set string
         */
        auto getText() const -> const std::string&;

        /**
         * \brief Enable or disable screen-space rendering
         */
        auto setScreenSpace(bool state) -> void;

        /**
         * \brief Retrieve screen-space rendering setting
         */
        auto getScreenSpace() const -> bool;

        /**
         * \brief Retrieve the asset Uuid of the font
         */
        auto getFont() const -> const Uuid&;

        /**
         * \brief Set the font resource
         */
        auto setFont(const Uuid& uuid) -> void;

        /**
         * \brief Set the colour of the text
         */
        auto setColour(Colour colour) -> void;

        /**
         * \brief Get the previously set text colour
         */
        auto getColour() -> Colour;

        /**
         * \brief Use an offset from the parent entity
         * \param offset
         */
        auto setOffset(glm::vec2 offset) -> void;

        /**
         * \brief Retrieve a previously set offset
         */
        auto getOffset() -> glm::vec2;

        /**
         * \brief Provide a pair of shaders to override the defaults
         */
        auto setShaders(const Uuid& vertex, const Uuid& fragment) -> void;

        /**
         * \brief Retrieve a previously set vertex shader
         */
        auto getVertexShader() -> const Uuid&;

        /**
         * \brief Retrieve a previously set fragment shader
         */
        auto getFragmentShader() -> const Uuid&;

    protected:
        auto draw(glm::mat4 transform) -> void;

    private:
        BitmapFont* m_font{nullptr};
        Uuid m_font_uuid;
        std::vector<Quad> m_quad_cache;
        bool m_screen_space{true};
        std::string m_text;
        Colour m_colour{1.F, 1.F, 1.F};
        glm::vec2         m_offset{0.F, 0.F};

        rosa::Uuid     m_vertex_shader{"00000000-0000-0000-0000-000000000001"};
        rosa::Uuid     m_fragment_shader{"00000000-0000-0000-0000-000000000002"};
        ShaderProgram* m_shader_program{nullptr};

        friend class SceneSerialiser;
        friend class Scene;
    };

} // namespace rosa