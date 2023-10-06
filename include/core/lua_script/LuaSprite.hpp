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

#include <SFML/Config.hpp>
#include <core/components/SpriteComponent.hpp>
#include <sol/sol.hpp>
#include <functional>

namespace rosa::lua_script {

    struct LuaSprite {
        LuaSprite(std::reference_wrapper<SpriteComponent> component, std::reference_wrapper<sol::state> state)
            : m_component(component), m_state(state) {}

        auto getTexture() const -> const std::string {
            return uuids::to_string(m_component.get().getTexture());
        }

        auto setTexture(std::string uuid_str) -> void {
            m_component.get().setTexture(uuids::uuid::from_string(uuid_str).value());
        }

        auto getColor() const -> sol::table {
            auto color = m_component.get().getColor();
            return m_state.get().create_table_with("r", color.r, "g", color.g, "b", color.g, "a", color.a);
        }

        auto setColor(int r, int g, int b, int a = 255) -> void {
            m_component.get().setColor(sf::Color(r, g, b, a));
        }

        private:
            std::reference_wrapper<SpriteComponent> m_component;
            std::reference_wrapper<sol::state> m_state;
    };

} // namespace rosa::lua_script