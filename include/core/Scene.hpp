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
//#include <core/Entity.hpp>
#include <core/Event.hpp>
#include <core/Uuid.hpp>

namespace rosa {

    class NativeScriptEntity;
    struct LuaScriptComponent;
    class SceneSerialiser;

    class Entity;

    /**
     * @brief Contains a registry of Entity and methods for handling them.
     * 
     *  It can be extended by your own class, see the testbed for an example scene.
     */
    class Scene {
        public:

            /**
             * @brief Construct a new Scene
             * 
             * @param render_window The OpenGL render window
             */
            explicit Scene(RenderWindow& render_window);

            Scene(Scene const &) = delete;
            auto operator=(Scene const &) -> Scene & = delete;
            Scene(Scene const &&) = delete;
            auto operator=(Scene const &&) -> Scene & = delete;

            /**
             * @brief Update is called each frame of the game loop.
             *
             *  This function calls into the update function of any LuaScript of NativeScript components.
             *  I also calculates heirarchical transforms and handles deferred entity deletions.
             *
             *  It can be overridden if you are deriving your own Scene class from here.
             * 
             * @param delta_time Seconds since the last frame update
             */
            virtual auto update(float delta_time) -> void;

            /**
             * @brief Calls draw methods once per frame, for entities that support them.
             */
            virtual auto render() -> void;

            /**
             * @brief Used to pass input events into entities with LuaScript or NativeScript components.
             * 
             * @param event 
             */
            virtual auto input(const Event& event) -> void;
            
            virtual ~Scene() = default;

            /**
             * @brief Get the render window
             * 
             * @return RenderWindow& reference to the render window
             */
            auto getRenderWindow() const -> RenderWindow& {
                return m_render_window;
            }

            /**
             * @brief Get the ECS registry
             * 
             * @return entt::registry& reference to the registry
             */
            auto getRegistry() -> entt::registry& {
                return m_registry;
            }

            /**
             * @brief Create a new entity in the scene.
             *
             *  The new entity will contain only a transform component
             * 
             * @return Entity& reference to a new entity
             */
            auto createEntity() -> Entity&;

            /**
             * @brief Remove an entity from the scene
             * 
             * @param entity reference to the entity to remove
             * @return true if the entity was removed
             * @return false if the entity didn't exist
             */
            auto removeEntity(Uuid uuid) -> bool;

            /**
             * @brief Get a specific entity
             * 
             * @param entt_id entt ID of the entity
             * @return Entity& reference to the entity
             */
            auto getEntity(Uuid uuid) -> Entity& {
                return m_entities.at(m_uuid_to_entity.at(uuid));
            }


        private:
            entt::registry m_registry{};
            RenderWindow& m_render_window;
            std::unordered_map<entt::entity, Entity> m_entities{};
            std::unordered_map<Uuid, entt::entity> m_uuid_to_entity{};
            std::unordered_map<entt::entity, Uuid> m_entity_to_uuid{};

            auto create_entity(Uuid uuid) -> Entity&;

            virtual auto onLoad() -> void {}
            virtual auto onUnload() ->  void {}

            friend class NativeScriptEntity;
            friend class Entity;
            friend class SceneSerialiser;
            friend struct LuaScriptComponent;
            friend class GameManager;

            double m_last_frame_time{};
    };

} // namespace rosa