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
#include <ecs/Entity.hpp>
#include <functional>

namespace rosa {

    class Scene;

    /**
     * \brief Represents a single object in the engine
     *
     * An Entity may have components and child entities. It may also be the child
     * of another Entity.
     */
    class Entity : public ecs::Entity {
    public:
        /**
         * \brief Construct an Entity
         * \param scene the Scene that has created this Entity
         *
         * UUID will be generated on construction
         */
        Entity(Scene* scene = nullptr)
            : ecs::Entity(Uuid::generate()), m_scene(scene) {}

        /**
         * \brief Construct an Entity from serialisation
         * \param uuid the previous UUID
         * \param scene the Scene that has create this Entity
         *
         * This constructor variant should be used when re-creating a previously
         * serialised entity.
         */
        Entity(const Uuid& uuid, Scene* scene = nullptr)
            : ecs::Entity(uuid), m_scene(scene) {}

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
            return getUuid() == other.getUuid();
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
         * \brief Get the UUID of the parent Entity
         * \return Uuid
         */
        auto getParent() -> const Uuid& {
            return m_parent;
        }

        auto setParent(const Uuid& parent_id) -> bool;
        auto removeParent() -> bool;

        /**
         * \brief Get the collection of children for this Entity
         * \return a vector of child references
         */
        auto getChildren() const -> const std::vector<Uuid>& {
            return m_children;
        }

        template<typename T>
        auto getComponent() -> T&;

        template<typename T>
        auto addComponent() -> T&;

        template<typename T>
        auto addComponent(T& data) -> T&;

        template<typename T>
        auto hasComponent() -> bool;

        template<typename T>
        auto removeComponent() -> bool;

        auto isActive() const -> bool override {
            return m_enabled;
        }

        auto setActive(bool active) -> void {
            m_enabled = active;
        }

    private:
        bool m_enabled{false};
        bool m_for_deletion{false};

        std::vector<Uuid> m_children{};
        Uuid m_parent = Uuid();

        friend class NativeScriptEntity;

        Scene* m_scene{nullptr};
        friend class Scene;
    };

} // namespace rosa