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
#include <glm/ext/matrix_clip_space.hpp>

namespace rosa {

    class SceneSerialiser;

    struct TextComponent {

        TextComponent()
            : m_shader_program(Renderer::getInstance().makeShaderProgram()) {
        }

        auto setText(const std::string& text) -> void {
            m_quad_cache.clear();
            m_text = text;
        }

        auto getText() const -> const std::string& {
            return m_text;
        }

        auto setScreenSpace(bool state) -> void {
            m_screen_space = state;
        }

        auto getScreenSpace() const -> bool {
            return m_screen_space;
        }

        auto getFont() const -> Uuid {
            return m_font_uuid;
        }

        auto setFont(Uuid uuid) -> void {
            m_font = &ResourceManager::getInstance().getAsset<BitmapFont>(uuid);
            m_font_uuid = uuid;
        }

        auto setColour(Colour colour) {
            m_colour = colour;
        }

        auto getColour() -> Colour {
            return m_colour;
        }

        auto render(glm::mat4 transform) -> void {

            if (m_quad_cache.empty()) {
                m_quad_cache = m_font->print(m_text, 0, 0, m_colour);
            }

            for (const auto& quad: m_quad_cache) {
                Renderable renderable{
                        quad,
                        glm::translate(transform, glm::vec3(quad.pos.x, quad.pos.y, 0.F)),
                        m_shader_program,
                        m_screen_space};
                Renderer::getInstance().submit(renderable);
            }
        }

    private:
        BitmapFont* m_font{nullptr};
        Uuid m_font_uuid;
        std::vector<Quad> m_quad_cache;
        bool m_screen_space{true};
        std::string m_text;
        Colour m_colour{1.F, 1.F, 1.F};

        ShaderProgram* m_shader_program{nullptr};

        friend class SceneSerialiser;
    };

} // namespace rosa