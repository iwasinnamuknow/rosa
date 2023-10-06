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
#include <sol/forward.hpp>
#define SOL_PRINT_ERRORS 0
#define SOL_ALL_SAFETIES_ON 1

#include <functional>
#include <sol/sol.hpp>
#include <stduuid/uuid.h>
#include <spdlog/spdlog.h>
#include <core/Scene.hpp>
#include <core/components/TransformComponent.hpp>
#include <core/lua_script/LuaTransform.hpp>

namespace rosa {

    class Scene;

    struct LuaScriptComponent {

            LuaScriptComponent(std::reference_wrapper<Scene> scene, entt::entity entity) : m_entity(entity), m_scene(scene) {
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

                        auto& tc = m_scene.get().getRegistry().get<TransformComponent>(m_entity);
                        auto lt = lua_script::LuaTransform(tc, m_state);
                        auto tn = m_state["transform"].get_or_create<sol::table>();
                        tn.set_function("getPosition", &lua_script::LuaTransform::getPosition, lt);
                        tn.set_function("setPosition", &lua_script::LuaTransform::setPosition, lt);
                        tn.set_function("getRotation", &lua_script::LuaTransform::getRotation, lt);
                        tn.set_function("setRotation", &lua_script::LuaTransform::setRotation, lt);

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

            entt::entity m_entity;
            std::reference_wrapper<Scene> m_scene;

            friend class Scene;
    };

} // namespace rosa