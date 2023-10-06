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
#include <memory>
#include <sol/forward.hpp>
#define SOL_PRINT_ERRORS 0
#define SOL_ALL_SAFETIES_ON 1

#include <functional>
#include <sol/sol.hpp>
#include <stduuid/uuid.h>
#include <spdlog/spdlog.h>
#include <core/Scene.hpp>
#include <core/components/TransformComponent.hpp>
#include <core/components/SpriteComponent.hpp>
#include <core/lua_script/LuaTransform.hpp>
#include <core/lua_script/LuaSprite.hpp>

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

                        auto& transform_component = m_scene.get().getRegistry().get<TransformComponent>(m_entity);
                        m_lua_transform = std::make_unique<lua_script::LuaTransform>(transform_component, m_state);
                        auto transform_table = m_state["transform"].get_or_create<sol::table>();
                        transform_table.set_function("getPosition", &lua_script::LuaTransform::getPosition, m_lua_transform.get());
                        transform_table.set_function("setPosition", &lua_script::LuaTransform::setPosition, m_lua_transform.get());
                        transform_table.set_function("getRotation", &lua_script::LuaTransform::getRotation, m_lua_transform.get());
                        transform_table.set_function("setRotation", &lua_script::LuaTransform::setRotation, m_lua_transform.get());

                        if (m_scene.get().getRegistry().all_of<SpriteComponent>(m_entity)) {
                            auto& sprite_component = m_scene.get().getRegistry().get<SpriteComponent>(m_entity);
                            m_lua_sprite = std::make_unique<lua_script::LuaSprite>(sprite_component, m_state);
                            auto sprite_table = m_state["sprite"].get_or_create<sol::table>();
                            sprite_table.set_function("getTexture", &lua_script::LuaSprite::getTexture, m_lua_sprite.get());
                            sprite_table.set_function("setTexture", &lua_script::LuaSprite::getTexture, m_lua_sprite.get());
                            sprite_table.set_function("getColor", &lua_script::LuaSprite::getColor, m_lua_sprite.get());
                            sprite_table.set_function("setColor", &lua_script::LuaSprite::setColor, m_lua_sprite.get());
                        }

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

            std::unique_ptr<lua_script::LuaTransform> m_lua_transform;
            std::unique_ptr<lua_script::LuaSprite> m_lua_sprite;

            friend class Scene;
    };

} // namespace rosa