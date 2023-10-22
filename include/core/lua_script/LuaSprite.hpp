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

#include <core/components/SpriteComponent.hpp>
#include <cstdint>
#include <sol/sol.hpp>
#include <functional>

namespace rosa::lua_script {

    struct LuaSprite {
        LuaSprite(std::reference_wrapper<SpriteComponent> component, std::reference_wrapper<sol::state> state)
            : m_component(component), m_state(state) {}

        auto getTexture() const -> std::string {
            return uuids::to_string(m_component.get().getTexture());
        }

        auto setTexture(const std::string& uuid_str) -> bool {
            auto uuid = uuids::uuid::from_string(uuid_str);
            if (uuid.has_value()) {
                m_component.get().setTexture(uuid.value());
                return true; // TODO we need to get a success value from component.setTexture and pass it through here
            }

            return false;            
        }

        auto getColour() const -> sol::table {
            auto color = m_component.get().getColour();
            return m_state.get().create_table_with("r", color.r, "g", color.g, "b", color.g, "a", color.a);
        }

        auto setColour(int red, int green, int blue, int alpha = 255) -> void {
            m_component.get().setColour(Colour(
                static_cast<std::uint8_t>(red), 
                static_cast<std::uint8_t>(green),
                static_cast<std::uint8_t>(blue), 
                static_cast<std::uint8_t>(alpha)
                )
            );
        }

        private:
            std::reference_wrapper<SpriteComponent> m_component;
            std::reference_wrapper<sol::state> m_state;
    };

} // namespace rosa::lua_script