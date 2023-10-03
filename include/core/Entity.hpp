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
#include <functional>

namespace rosa {

    class Entity {
        public:
            Entity(entt::entity ent_id, std::reference_wrapper<entt::registry> registry) : m_id(ent_id), m_registry(registry) {}
            Entity(Entity&&) = default;
            Entity(const Entity&) = default;
            auto operator=(Entity const &) -> Entity & = default;
            auto operator=(Entity&&) -> Entity & = default;
            ~Entity() = default;

            operator entt::entity() const {
                return m_id;
            }

            template<typename T>
            auto getComponent() -> T& {
                assert(hasComponent<T>());
                return m_registry.get().get<T>(m_id);
            }

            template<typename T>
            auto hasComponent() -> bool {
                return m_registry.get().any_of<T>(m_id);
            }

            template<typename T, typename... Args>
            auto addComponent(Args&&... args) -> T& {
                assert(!hasComponent<T>());
                return m_registry.get().emplace<T>(m_id, std::forward<Args>(args)...);
            }

            template<typename T>
            auto removeComponent() -> bool {
                assert(hasComponent<T>());
                return m_registry.get().remove<T>(m_id);
            }

            auto getId() const -> entt::entity {
                return m_id;
            }

            auto forDeletion() const -> bool {
                return m_for_deletion;
            }

            auto die() -> void {
                m_for_deletion = true;
            }

        private:
            entt::entity m_id = entt::null;
            bool m_enabled{true};
            bool m_for_deletion{false};

            std::reference_wrapper<entt::registry> m_registry;
            
            friend class NativeScriptEntity;
            friend class Scene;
    };

} // namespace rosa