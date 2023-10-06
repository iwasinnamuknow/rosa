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

#include <memory>
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
    class SceneSerialiser;

    struct LuaScriptComponent {

            LuaScriptComponent(std::reference_wrapper<Scene> scene, entt::entity entity);
            auto setScript(uuids::uuid uuid) -> bool;

        private:
            sol::protected_function m_on_create_function;
            sol::protected_function m_on_delete_function;
            sol::protected_function m_on_update_function;
            uuids::uuid m_uuid;
            sol::state m_state;

            entt::entity m_entity;
            std::reference_wrapper<Scene> m_scene;

            std::unique_ptr<lua_script::LuaTransform> m_lua_transform;
            std::unique_ptr<lua_script::LuaSprite> m_lua_sprite;

            friend class Scene;
            friend class SceneSerialiser;
    };

} // namespace rosa