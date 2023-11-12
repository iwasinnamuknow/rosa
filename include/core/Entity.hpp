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

#include "core/components/TransformComponent.hpp"
#include <entt/entt.hpp>
#include <core/Uuid.hpp>
#include <functional>
#include <core/Scene.hpp>

namespace rosa {

    class Entity {
        public:
            Entity(entt::entity ent_id, Scene& scene) : m_id(ent_id), m_scene(scene) {
                m_uuid = Uuid::generate();
            }

            Entity(Uuid uuid, entt::entity ent_id, Scene& scene) : m_id(ent_id), m_uuid(uuid), m_scene(scene) {}

            Entity(Entity&&) = default;
            Entity(const Entity&) = default;
            auto operator=(Entity const &) -> Entity & = default;
            auto operator=(Entity&&) -> Entity & = default;
            ~Entity() = default;

            operator entt::entity() const {
                return m_id;
            }

            /**
             * @brief Get a specific component
             * 
             * @tparam T Component type
             * @return T& reference to the component
             */
            template<typename T>
            auto getComponent() -> T& {
                assert(hasComponent<T>());
                return m_scene.getRegistry().get<T>(m_id);
            }

            /**
             * @brief Enquire if the Entity contains a specific component type
             * 
             * @tparam T Component type
             * @return true if the component exists
             * @return false otherwise.
             */
            template<typename T>
            auto hasComponent() -> bool {
                return m_scene.getRegistry().any_of<T>(m_id);
            }

            /**
             * @brief Add a specific component type to the Entity
             * 
             * @tparam T Component type
             * @return T& reference to the created component
             */
            template<typename T, typename... Args>
            auto addComponent(Args&&... args) -> T& {
                assert(!hasComponent<T>());
                return m_scene.getRegistry().emplace<T>(m_id, std::forward<Args>(args)...);
            }

            /**
             * @brief Remove a specific component type from the Entity
             * 
             * @tparam T Component type
             * @return true if the component was removed
             * @return false otherwise.
             */
            template<typename T>
            auto removeComponent() -> bool {
                assert(hasComponent<T>());
                return m_scene.getRegistry().remove<T>(m_id);
            }

            /**
             * @brief Get the entt identifier of the Entity
             * 
             * @return entt::entity identifier
             */
            auto getId() const -> entt::entity {
                return m_id;
            }

            /**
             * @brief Get the UUID identifier of the Entity
             * 
             * @return Uuid
             */
            auto getUUID() const -> Uuid {
                return m_uuid;
            }

            /**
             * @brief Check if the Entity is due for deletion on the next frame
             * 
             * @return true the Entity will be deleted
             * @return false no deletion will happen
             */
            auto forDeletion() const -> bool {
                return m_for_deletion;
            }

            /**
             * @brief Queues the Entity for destruction on the next frame
             */
            auto die() -> void {
                m_for_deletion = true;
            }

            /**
             * @brief Set this Entity as the child of another for transforms
             * 
             * @param parent_id entt identifier of the parent Entity
             * @return true relationship was created
             * @return false relationship was not created (empty parent_id?)
             */
            auto setParent(Uuid parent_id) -> bool;

            /**
             * @brief Remove this Entity as the child of another
             * 
             * @return true relationship was removed
             * @return false relationship was not removed (is there a parent to remove?)
             */
            auto removeParent() -> bool;

            /**
             * @brief Get the entt identifier of the parent Entity 
             * 
             * @return entt::entity identifier
             */
            auto getParent() -> Uuid {
                return m_parent;
            }

            /**
             * @brief Get the collection of children for this Entity
             * 
             * @return const std::vector<entt::entity>& A vector of child references
             */
            auto getChildren() const -> const std::vector<Uuid>& {
                return m_children;
            }

        private:
            entt::entity m_id = entt::null;
            bool m_enabled{true};
            bool m_for_deletion{false};
            Uuid m_uuid;

            std::vector<Uuid> m_children{};
            Uuid m_parent = Uuid();

            Scene& m_scene;
            
            friend class NativeScriptEntity;
            friend class Scene;
    };

} // namespace rosa