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
 *  You should have received a copy of the GNU General Public License along with rosa. If not,
 *  see <https://www.gnu.org/licenses/>.
 */

#pragma once

#include <core/ResourceManager.hpp>
#include <ecs/EntityRegistry.hpp>
#include <graphics/RenderWindow.hpp>
#include <spdlog/spdlog.h>
#include <unordered_map>

#include <core/Entity.hpp>
#include <core/Event.hpp>

namespace rosa {

    class NativeScriptEntity;
    struct LuaScriptComponent;
    class SceneSerialiser;

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
            explicit Scene(RenderWindow* render_window = nullptr);

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
                return *m_render_window;
            }

            /**
             * @brief Get the ECS registry
             */
            auto getRegistry() -> ecs::EntityRegistry<Entity>& {
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
             * @brief Create a new entity in the scene with a predetermined uuid.
             *
             *  The new entity will contain only a transform component
             * 
             * @param Uuid A predetermined uuid
             * @return Entity& reference to a new entity
             */
            auto createEntity(const Uuid& uuid) -> Entity&;

            /**
             * @brief Defer a function call until the next frame
             * \param fn A lambda object
             *
             * Example:
             *
             * deferCall([this]() -> void {
             *     auto& entity = this->getScene().createEntity();
             *     entity.setActive(false);
             *     ...
             * }
             */
            auto deferCall(std::function<void()> fn) -> void;

            /**
             * @brief Remove an entity from the scene
             * 
             * @param entity reference to the entity to remove
             * @return true if the entity was removed
             * @return false if the entity didn't exist
             */
            auto removeEntity(const Uuid& uuid) -> bool;

            /**
             * @brief Get a specific entity
             */
            auto getEntity(const Uuid& uuid) -> Entity&;

        private:
            ecs::EntityRegistry<Entity> m_registry;
            RenderWindow* m_render_window;

            virtual auto onLoad() -> void {}
            virtual auto onUnload() ->  void {}

            friend class NativeScriptEntity;
            friend class Entity;
            friend class SceneSerialiser;
            friend struct LuaScriptComponent;
            friend class GameManager;

            double m_last_frame_time{};
            glm::vec4 m_active_camera_pos{0};

            std::vector<std::function<void()>> m_deferred{};
    };

} // namespace rosa