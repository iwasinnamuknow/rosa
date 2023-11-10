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

#include <memory>
#include <functional>
#include <sol/sol.hpp>
#include <core/Uuid.hpp>
#include <spdlog/spdlog.h>
#include <core/Scene.hpp>
#include <core/components/TransformComponent.hpp>
#include <core/components/SpriteComponent.hpp>
// #include <core/lua_script/LuaTransform.hpp>
// #include <core/lua_script/LuaSprite.hpp>
// #include <core/lua_script/LuaSound.hpp>
// #include <core/lua_script/LuaMusic.hpp>

namespace rosa {

    class Scene;
    class SceneSerialiser;

    struct LuaScriptComponent {

            LuaScriptComponent(Scene* scene, entt::entity entity);
            auto setScript(Uuid uuid, bool deserialised = false) -> bool;

        private:
            sol::state m_state;
            sol::protected_function m_on_create_function;
            sol::protected_function m_on_load_function;
            sol::protected_function m_on_delete_function;
            sol::protected_function m_on_update_function;
            sol::protected_function m_on_input_function;
            Uuid m_uuid;

            auto set_data(const std::string& key, sol::table& table) -> void;

            entt::entity m_entity;
            Scene* m_scene;

            //std::unique_ptr<lua_script::LuaTransform> m_lua_transform;
            //std::unique_ptr<lua_script::LuaSprite> m_lua_sprite;
            //std::unique_ptr<lua_script::LuaSound> m_lua_sound;
            //std::unique_ptr<lua_script::LuaMusic> m_lua_music;

            friend class Scene;
            friend class SceneSerialiser;
    };

} // namespace rosa