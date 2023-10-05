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
#include <core/Scene.hpp>
#include <core/components/TransformComponent.hpp>

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

                        auto transform = m_state["transform"].get_or_create<sol::table>();
                        transform.set_function("getPosition", [this]() -> sol::table {
                            auto& tc = m_scene.get().getRegistry().get<TransformComponent>(m_entity);
                            return m_state.create_table_with("x", tc.position.x, "y", tc.position.y);
                        });
                        transform.set_function("setPosition", [this](float x, float y) -> void {
                            auto& tc = m_scene.get().getRegistry().get<TransformComponent>(m_entity);
                            tc.position = sf::Vector2f(x, y);
                        });
                        transform.set_function("getRotation", [this]() -> float {
                            auto& tc = m_scene.get().getRegistry().get<TransformComponent>(m_entity);
                            return tc.rotation;
                        });
                        transform.set_function("setRotation", [this](float rotation) -> void {
                            auto& tc = m_scene.get().getRegistry().get<TransformComponent>(m_entity);
                            tc.rotation = rotation;
                        });

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