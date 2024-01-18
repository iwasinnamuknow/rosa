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

#include <core/Uuid.hpp>
#include <core/ResourceManager.hpp>
#include <graphics/BitmapFont.hpp>
#include <graphics/Quad.hpp>
#include <graphics/Renderer.hpp>
#include <glm/glm.hpp>
#include <glm/common.hpp>

#include <string_view>
#include <vector>
#include <glm/ext/matrix_clip_space.hpp>

namespace rosa {

    class SceneSerialiser;

    struct TextComponent {

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

        auto render(glm::mat4 world_projection, int screen_width = 0, int screen_height = 0) -> void {

            if (m_quad_cache.empty()) {

                float world_x = world_projection[3].x;
                float world_y = world_projection[3].y;

                m_quad_cache = m_font->print(m_text, world_x, world_y, m_colour);
            }

            if (m_screen_space) {
                auto projection = glm::ortho(0, screen_width, screen_height, 0, -1, 1);
                for (const auto &quad: m_quad_cache) {
                    Renderer::getInstance().submit(quad, projection, m_screen_space);
                }
            } else {
                for (const auto &quad: m_quad_cache) {
                    Renderer::getInstance().submitForBatch(quad);
                }
            }
        }

    private:
        BitmapFont* m_font{nullptr};
        Uuid m_font_uuid;
        std::vector<Quad> m_quad_cache;
        bool m_screen_space{true};
        std::string m_text;
        Colour m_colour{1.F, 1.F, 1.F};

        friend class SceneSerialiser;
    };

} // namespace rosa