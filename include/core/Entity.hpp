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

#include <core/Uuid.hpp>
#include <core/components/TransformComponent.hpp>
#include <entt/entt.hpp>
#include <functional>

namespace rosa {

    class Scene;

    /**
     * \brief Represents a single object in the engine
     *
     * An Entity may have components and child entities. It may also be the child
     * of another Entity.
     */
    class Entity {
    public:
        /**
         * \brief Construct an Entity
         * \param ent_id the Entt registry id
         * \param scene the Scene that has created this Entity
         *
         * UUID will be generated on construction
         */
        Entity(entt::entity ent_id, Scene& scene) : m_id(ent_id), m_uuid(Uuid::generate()), m_scene(scene) {}

        /**
         * \brief Construct an Entity from serialisation
         * \param uuid the previous UUID
         * \param ent_id the Entt registry id
         * \param scene the Scene that has create this Entity
         *
         * This constructor variant should be used when re-creating a previously
         * serialised entity.
         */
        Entity(Uuid uuid, entt::entity ent_id, Scene& scene) : m_id(ent_id), m_uuid(uuid), m_scene(scene) {}

        /**
         * \brief Copy constructor
         */
        Entity(const Entity&) = default;

        /**
         * \brief Copy assignment operator
         */
        auto operator=(Entity const&) -> Entity& = default;

        /**
         * \brief Move constructor
         */
        Entity(Entity&&) = default;

        /**
         * \brief Move assignment operator
         */
        auto operator=(Entity&&) -> Entity& = default;

        /**
         * \brief Default destructor
         */
        ~Entity() = default;

        /**
         * \brief Equality operator
         * \param other another Entity to compare with
         * \return true if matching, false if not
         */
        auto operator==(const Entity& other) const -> bool {
            return getUUID() == other.getUUID();
        }

        /**
         * \brief Cast operator for Entt
         */
        operator entt::entity() const {
            return m_id;
        }

        /**
         * \brief Get a specific component
         * \tparam T Component type
         * \return reference to the component
         */
        template<typename T>
        auto getComponent() -> T&;

        /**
         * \brief Enquire if the Entity contains a specific component type
         * \tparam T Component type
         * \return true if the component exists, false otherwise
         */
        template<typename T>
        auto hasComponent() -> bool;

        /**
         * \brief Add a specific component type to the Entity
         * \tparam T Component type
         * \return reference to the created component
         */
        template<typename T>
        auto addComponent() -> T&;

        /**
         * \brief Remove a specific component type from the Entity
         * \tparam T Component type
         * \return true if the component was removed, false otherwise
         */
        template<typename T>
        auto removeComponent() -> bool;

        /**
         * \brief Get the entt id of the Entity
         * \return entt::entity identifier
         */
        auto getId() const -> entt::entity {
            return m_id;
        }

        /**
         * \brief Get the UUID of the Entity
         * \return Uuid
         */
        auto getUUID() const -> Uuid {
            return m_uuid;
        }

        /**
         * \brief Check if the Entity is due for deletion on the next frame
         * \return true the Entity will be deleted
         * \return false no deletion will happen
         */
        auto forDeletion() const -> bool {
            return m_for_deletion;
        }

        /**
         * \brief Queues the Entity for destruction on the next frame
         */
        auto die() -> void {
            m_for_deletion = true;
        }

        /**
         * \brief Set this Entity as the child of another
         * \param parent_id UUID of the parent Entity
         * \return true relationship was created
         * \return false relationship was not created (empty parent_id?)
         */
        auto setParent(Uuid parent_id) -> bool;

        /**
         * \brief Orphan this entity
         * \return true relationship was removed
         * \return false relationship was not removed (is there a parent to remove?)
         */
        auto removeParent() -> bool;

        /**
         * \brief Get the UUID of the parent Entity
         * \return Uuid
         */
        auto getParent() -> Uuid {
            return m_parent;
        }

        /**
         * \brief Get the collection of children for this Entity
         * \return a vector of child references
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