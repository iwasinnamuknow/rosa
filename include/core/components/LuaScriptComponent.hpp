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
#define SOL_PRINT_ERRORS 0
#define SOL_ALL_SAFETIES_ON 1

#include <functional>
#include <sol/sol.hpp>
#include <stduuid/uuid.h>
#include <spdlog/spdlog.h>

namespace rosa {

    class Scene;

    struct LuaScriptComponent {

            LuaScriptComponent() {
                m_state.open_libraries(sol::lib::base);
            }

            auto setScript(const std::string& filepath) -> bool {
                //auto handler = &sol::script_default_on_error;

                try {
                    auto result = m_state.script_file(filepath, &sol::script_default_on_error);
                    if(result.valid()) {
                        m_on_create_function = m_state["onCreate"];
                        m_on_delete_function = m_state["onDelete"];
                        m_on_update_function = m_state["onUpdate"];

                        m_on_create_function();

                        return true;
                    }

                } catch (sol::error& e) {
                    spdlog::error("Couldn't load lua script: {}", e.what());
                }
                
                return false;
            }

        private:
            sol::protected_function m_on_create_function;
            sol::protected_function m_on_delete_function;
            sol::protected_function m_on_update_function;
            std::string m_script;
            sol::state m_state;

            friend class Scene;
    };

} // namespace rosa