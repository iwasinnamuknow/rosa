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

#include <array>
#include <cstdint>
#include <ecs/ComponentRegistry.hpp>
#include <ecs/Entity.hpp>
#include <ecs/EntityArray.hpp>
#include <memory>

namespace rosa::ecs {
    template<class C>
    class EntityRegistry {
    public:
        EntityRegistry()
            : m_entities{std::make_unique<EntityArray<C>>()} {
        }

        auto createEntity(rosa::Uuid uuid = rosa::Uuid::generate()) -> C& {
            C& entity = m_entities->createEntity(uuid);
            entity.setActive(true);
            return entity;
        }

        auto removeEntity(rosa::Uuid uuid) -> void {
            m_entities->removeEntity(uuid);
        }

        auto getEntity(rosa::Uuid uuid) -> C& {
            return m_entities->getEntity(uuid);
        }

        template<typename T>
        auto addComponent(rosa::Uuid uuid) -> T& {
            auto& entity    = m_entities->getEntity(uuid);
            T&    component = m_component_registry.addComponent<T>(uuid);

            entity.getComponentSignature().set(static_cast<size_t>(m_component_registry.getComponentType<T>()));
            return component;
        }

        template<typename T>
        auto addComponent(rosa::Uuid uuid, T& data) -> T& {
            auto& entity    = m_entities->getEntity(uuid);
            T&    component = m_component_registry.addComponent<T>(uuid, data);

            entity.getComponentSignature().set(static_cast<size_t>(m_component_registry.getComponentType<T>()));
            return component;
        }

        template<typename T>
        auto removeComponent(rosa::Uuid uuid) -> void {
            auto& entity = m_entities->getEntity(uuid);
            m_component_registry.removeComponent<T>(uuid);
            entity.getComponentSignature().set(static_cast<size_t>(m_component_registry.getComponentType<T>()), false);
        }

        template<typename T>
        auto getComponent(rosa::Uuid uuid) -> T& {
            return m_component_registry.getComponent<T>(uuid);
        }

        template<typename T>
        auto hasComponent(rosa::Uuid uuid) -> bool {
            return m_entities->getEntity(uuid).getComponentSignature().test(static_cast<size_t>(m_component_registry.getComponentType<T>()));
        }

        template<typename T>
        auto getComponentType() -> component_id {
            return m_component_registry.getComponentType<T>();
        }

        template<typename T>
        auto registerComponent() -> void {
            m_component_registry.registerComponent<T>();
        }

        auto getAtIndex(size_t index) -> Entity& {
            return m_entities->getAtIndex(index);
        }

        auto count() const -> size_t {
            return m_entities->count();
        }

    private:
        // entity storage
        std::unique_ptr<EntityArray<C>> m_entities{nullptr};

        // component registry
        ComponentRegistry m_component_registry{};
    };

}// namespace rosa::ecs