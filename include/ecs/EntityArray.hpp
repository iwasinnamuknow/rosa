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
#include <unordered_map>
#include <cassert>
#include <ecs/Entity.hpp>

namespace rosa::ecs {

    template<class T>
    class EntityArray {
    public:
        auto createEntity(rosa::Uuid uuid) -> T& {
            assert(m_size < max_entities && "Max entities reached.");
            assert(m_uuid_to_index.find(uuid) == m_uuid_to_index.end() && "Uuid is already registered for an entity.");

            // Put new entry at end and update the maps
            size_t new_index = m_size;
            m_uuid_to_index[uuid] = new_index;
            m_index_to_uuid[new_index] = uuid;
            m_entities[new_index] = T(uuid);
            ++m_size;

            return m_entities[new_index];
        }

        auto removeEntity(rosa::Uuid uuid)-> void {

            assert(m_uuid_to_index.find(uuid) != m_uuid_to_index.end() && "Removing non-existent entity.");

            // move element at end into deleted element's place to maintain density
            size_t index_of_removed = m_uuid_to_index[uuid];
            size_t index_of_last = m_size - 1;
            m_entities[index_of_removed] = m_entities[index_of_last];

            // Update map to point to moved spot
            rosa::Uuid uuid_of_last = m_index_to_uuid[index_of_last];
            m_uuid_to_index[uuid_of_last] = index_of_removed;
            m_index_to_uuid[index_of_removed] = uuid_of_last;

            m_uuid_to_index.erase(uuid);
            m_index_to_uuid.erase(index_of_last);

            --m_size;
        }

        auto getEntity(rosa::Uuid uuid)-> T& {
            assert(m_uuid_to_index.find(uuid) != m_uuid_to_index.end() && "Retrieving non-existent entity.");

            // Return a reference to the entity's component
            return m_entities[m_uuid_to_index[uuid]];
        }

        auto getAtIndex(size_t index) -> T& {
            //assert(index < max_entities && "Entity index out of range");
            return m_entities[index];
        }

        auto count() const -> size_t {
            return m_size;
        }

    private:
        // The packed array of components (of generic type T),
        // set to a specified maximum amount, matching the maximum number
        // of entities allowed to exist simultaneously, so that each entity
        // has a unique spot.
        std::array<T, max_entities> m_entities{};

        // Map from a uuid to an array index.
        std::unordered_map<rosa::Uuid, size_t> m_uuid_to_index{};

        // Map from an array index to a uuid
        std::unordered_map<size_t, rosa::Uuid> m_index_to_uuid{};

        // Total size of valid entries in the array.
        size_t m_size{0};
    };

} // namespace rosa::ecs