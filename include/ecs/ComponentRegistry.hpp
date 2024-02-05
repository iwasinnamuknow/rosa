/*
*  This file is part of rosa.
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

#include <cstdint>
#include <string>
#include <unordered_map>
#include <memory>
#include <cassert>
#include <queue>
#include <ecs/Component.hpp>
#include <ecs/ComponentArray.hpp>

namespace rosa::ecs {

    class ComponentRegistry {
    public:
        ComponentRegistry() {
            for (component_id i = 1; i <= max_components; i++) {
                m_type_ids.push(i);
            }
        }

        ~ComponentRegistry() = default;

        template<typename T>
        auto registerComponent() -> void {

            assert(m_types.size() < max_components && "Too many components added.");

            const char* type_name = typeid(T).name();
            assert(m_types.find(type_name) == m_types.end() && "Registering component type more than once.");
            m_types.insert({type_name, m_type_ids.front()});
            // Create a ComponentArray pointer and add it to the component arrays map
            m_component_arrays.insert({m_type_ids.front(), std::make_unique<ComponentArray<T>>()});
            m_type_ids.pop();
        }

        template<typename T>
        auto getComponentType() const -> component_id {
            const char* type_name = typeid(T).name();
            assert(m_types.find(type_name) != m_types.end() && "Component not registered before use.");
            return m_types.at(type_name);
        }

        template<typename T>
        auto addComponent(rosa::Uuid uuid) -> T& {
            // Add a component to the array for an entity
            getComponentArray<T>()->addData(uuid);
            return getComponent<T>(uuid);
        }

        template<typename T>
        auto addComponent(rosa::Uuid uuid, T& data) -> T& {
            // Add a component to the array for an entity
            getComponentArray<T>()->addData(uuid, data);
            return getComponent<T>(uuid);
        }

        template<typename T>
        void removeComponent(rosa::Uuid uuid) {
            // Remove a component from the array for an entity
            getComponentArray<T>()->removeData(uuid);
        }

        template<typename T>
        auto getComponent(rosa::Uuid uuid) -> T& {
            // Get a reference to a component from the array for an entity
            return getComponentArray<T>()->getData(uuid);
        }

        auto onEntityDestroyed(rosa::Uuid uuid) -> void {
            // Notify each component array that an entity has been destroyed
            // If it has a component for that entity, it will remove it
            for (auto const& pair: m_component_arrays) {
                auto const& component = pair.second;
                component->onEntityDestroyed(uuid);
            }
        }


    private:
        // Component type ids to be claimed
        std::queue<component_id> m_type_ids{};

        // Component type id map
        std::unordered_map<const char*, component_id> m_types{};

        // Component array storage
        std::unordered_map<component_id, std::unique_ptr<IComponentArray>> m_component_arrays{};

        // Convenience function to get the statically cast pointer to the ComponentArray of type T.
        template<typename T>
        ComponentArray<T>* getComponentArray() {
            const char* type_name = typeid(T).name();

            assert(m_types.find(type_name) != m_types.end() && "Component not registered before use.");

            component_id type_id = m_types[type_name];

            auto* array = dynamic_cast<ComponentArray<T>*>((m_component_arrays[type_id]).get());

            return array;
        }

    };

} // namespace rosa::ecs