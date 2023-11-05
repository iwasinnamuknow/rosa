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

#include <core/components/MusicPlayerComponent.hpp>
#include <cstdint>
#include <sol/sol.hpp>
#include <functional>

namespace rosa::lua_script {

    struct LuaMusic {
        LuaMusic(std::reference_wrapper<MusicPlayerComponent> component, std::reference_wrapper<sol::state> state)
            : m_component(component), m_state(state) {}

        auto getAudio() -> std::string {
            return uuids::to_string(m_component.get().getAudio());
        }

        auto setAudio(const std::string& uuid_str) -> bool {
            auto uuid = uuids::uuid::from_string(uuid_str);
            if (uuid.has_value()) {
                m_component.get().setAudio(uuid.value());
                return true;
            }

            return false;            
        }

        auto setVolume(float volume) -> void {
            m_component.get().setVolume(volume);
        }

        auto play() -> void {
            m_component.get().play();
        }

        auto stop() -> void {
            m_component.get().stop();
        }

        auto setLooping(bool loop) {
            m_component.get().setLooping(loop);
        }

        private:
            std::reference_wrapper<MusicPlayerComponent> m_component;
            std::reference_wrapper<sol::state> m_state;
    };

} // namespace rosa::lua_script