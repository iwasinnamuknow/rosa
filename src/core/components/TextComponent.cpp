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

#include <core/components/TextComponent.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace rosa {

    auto TextComponent::setText(const std::string& text) -> void {
        m_quad_cache.clear();
        m_text = text;
    }

    auto TextComponent::draw(glm::mat4 transform) -> void {

        if (m_quad_cache.empty()) {
            m_quad_cache = m_font->print(m_text, 0, 0, m_colour);
        }

        for (const auto& quad: m_quad_cache) {
            Renderable renderable{
                    quad,
                    glm::translate(transform, glm::vec3(quad.pos.x + m_offset.x, quad.pos.y + m_offset.y, 0.F)),
                    m_shader_program,
                    m_screen_space};
            Renderer::getInstance().submit(renderable);
        }
    }

    auto TextComponent::getText() const -> const std::string& {
        return m_text;
    }

    auto TextComponent::setScreenSpace(bool state) -> void {
        m_screen_space = state;
    }

    auto TextComponent::getScreenSpace() const -> bool {
        return m_screen_space;
    }

    auto TextComponent::getFont() const -> const Uuid& {
        return m_font_uuid;
    }

    auto TextComponent::setFont(const Uuid& uuid) -> void {
        m_font      = &ResourceManager::getInstance().getAsset<BitmapFont>(uuid);
        m_font_uuid = uuid;
    }

    auto TextComponent::setColour(Colour colour) -> void {
        m_colour = colour;
    }

    auto TextComponent::getColour() -> Colour {
        return m_colour;
    }

    auto TextComponent::setOffset(glm::vec2 offset) -> void {
        m_offset = offset;
    }

    auto TextComponent::getOffset() -> glm::vec2 {
        return m_offset;
    }

    auto TextComponent::setShaders(const Uuid& vertex, const Uuid& fragment) -> void {
        m_vertex_shader   = vertex;
        m_fragment_shader = fragment;

        m_shader_program = Renderer::getInstance().makeShaderProgram(m_vertex_shader, m_fragment_shader);
    }

    auto TextComponent::getVertexShader() -> const Uuid& {
        return m_vertex_shader;
    }

    auto TextComponent::getFragmentShader() -> const Uuid& {
        return m_fragment_shader;
    }

}// namespace rosa
