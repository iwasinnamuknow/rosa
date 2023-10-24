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

#include <entt/entt.hpp>
#include <core/ResourceManager.hpp>
#include <graphics/RenderWindow.hpp>
#include <spdlog/spdlog.h>
#include <unordered_map>
#include <core/Entity.hpp>
#include <debug/Profiler.hpp>
#include <core/Event.hpp>

namespace rosa {

    class NativeScriptEntity;
    struct LuaScriptComponent;
    class SceneSerialiser;

    namespace lua_script {
        struct LuaTransform;
    } // namespace lua_script

    class Scene {
        public:
            explicit Scene(RenderWindow& render_window);
            virtual auto update(float delta_time) -> void;
            virtual auto render() -> void;
            virtual auto input(const Event& event) -> void;
            virtual ~Scene() = default;

            auto getRenderWindow() const -> RenderWindow& {
                return m_render_window;
            }

        protected:
            auto getRegistry() -> entt::registry& {
                return m_registry;
            }

            auto createEntity() -> Entity&;
            auto removeEntity(Entity& entity) -> bool;
            auto getEntity(entt::entity entt_id) -> Entity& {
                return m_entities.at(entt_id);
            }


        private:
            entt::registry m_registry{};
            RenderWindow& m_render_window;
            std::unordered_map<entt::entity, Entity> m_entities{};

            auto create_entity(uuids::uuid uuid) -> Entity&;

            virtual auto onLoad() -> void {}
            virtual auto onUnload() ->  void {}

            friend class NativeScriptEntity;
            friend class Entity;
            friend class SceneSerialiser;
            friend struct LuaScriptComponent;
            friend struct lua_script::LuaTransform;
            friend class GameManager;

            auto show_profile_stats(bool* open) -> void;
            bool m_show_profile_stats{true};

            float m_last_frame_time;
    };

} // namespace rosa