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

#include <cassert>
#include <core/Uuid.hpp>
#include <ecs/Component.hpp>
#include <ecs/Entity.hpp>
#include <unordered_map>

namespace rosa::ecs {

    class IComponentArray {
    public:
        virtual ~IComponentArray()                          = default;
        virtual void onEntityDestroyed(const rosa::Uuid& /*uuid*/) = 0;
    };

    template<typename T>
    class ComponentArray : public IComponentArray {
    public:
        auto addData(const rosa::Uuid& uuid) -> T& {
            assert(m_uuid_to_index.find(uuid) == m_uuid_to_index.end() && "Component added to same entity more than once.");

            // Put new entry at end and update the maps
            size_t new_index           = m_size;
            m_uuid_to_index[uuid]      = new_index;
            m_index_to_uuid[new_index] = uuid;
            m_components[new_index]    = std::move(T());
            ++m_size;

            return m_components[new_index];
        }

        auto addData(const rosa::Uuid& uuid, T& data) -> T& {
            assert(m_uuid_to_index.find(uuid) == m_uuid_to_index.end() && "Component added to same entity more than once.");

            // Put new entry at end and update the maps
            size_t new_index           = m_size;
            m_uuid_to_index[uuid]      = new_index;
            m_index_to_uuid[new_index] = uuid;
            m_components[new_index]    = std::move(data);
            ++m_size;

            return m_components[new_index];
        }

        auto removeData(const rosa::Uuid& uuid) -> void {
            assert(m_uuid_to_index.find(uuid) != m_uuid_to_index.end() && "Removing non-existent component.");

            // Copy element at end into deleted element's place to maintain density
            size_t index_of_removed        = m_uuid_to_index[uuid];
            size_t index_of_last           = m_size - 1;
            m_components[index_of_removed] = std::move(m_components[index_of_last]);

            // Update map to point to moved spot
            rosa::Uuid uuid_of_last           = m_index_to_uuid[index_of_last];
            m_uuid_to_index[uuid_of_last]     = index_of_removed;
            m_index_to_uuid[index_of_removed] = uuid_of_last;

            m_uuid_to_index.erase(uuid);
            m_index_to_uuid.erase(index_of_last);

            --m_size;
        }

        auto getData(const rosa::Uuid& uuid) -> T& {
            assert(m_uuid_to_index.find(uuid) != m_uuid_to_index.end() && "Retrieving non-existent component.");

            // Return a reference to the entity's component
            return m_components[m_uuid_to_index[uuid]];
        }

        void onEntityDestroyed(const rosa::Uuid& uuid) override {
            if (m_uuid_to_index.find(uuid) != m_uuid_to_index.end()) {
                // Remove the entity's component if it existed
                removeData(uuid);
            }
        }

    private:
        // The packed array of components (of generic type T),
        // set to a specified maximum amount, matching the maximum number
        // of entities allowed to exist simultaneously, so that each entity
        // has a unique spot.
        std::array<T, max_entities> m_components{};

        // Map from an entity ID to an array index.
        std::unordered_map<rosa::Uuid, size_t> m_uuid_to_index;

        // Map from an array index to an entity ID.
        std::unordered_map<size_t, rosa::Uuid> m_index_to_uuid;

        // Total size of valid entries in the array.
        size_t m_size{0};
    };

}// namespace rosa::ecs