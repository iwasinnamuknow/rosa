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

#include <core/components/TransformComponent.hpp>
#include <core/Scene.hpp>
#include <sol/forward.hpp>
#include <sol/sol.hpp>
#include <functional>
#include <sol/state.hpp>

namespace rosa::lua_script {

    struct LuaTransform {
        LuaTransform(std::reference_wrapper<TransformComponent> component, std::reference_wrapper<sol::state> state)
            : m_component(component), m_state(state) {}

        auto getPosition() const -> sol::table {
            return m_state.get().create_table_with("x", m_component.get().position.x, "y", m_component.get().position.y);
        }

        auto setPosition(float x, float y) -> void {
            m_component.get().setPosition(x, y);
        }

        // auto getRotation() const -> float {
        //     return m_component.get().rotation;
        // }

        // auto setRotation(float rotation) -> void {
        //     m_component.get().rotation = rotation;
        // }

        private:
            std::reference_wrapper<TransformComponent> m_component;
            std::reference_wrapper<sol::state> m_state;
    };

} // namespace rosa::lua_script