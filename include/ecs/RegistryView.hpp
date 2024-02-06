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

#include <ecs/EntityRegistry.hpp>

namespace rosa::ecs {

    template<class C, typename... ComponentTypes>
    class RegistryView {
    public:
        struct Iterator {
            Iterator(EntityRegistry<C>* pregistry, size_t pindex, ec_sig pmask, bool pall)
                : index(pindex), registry(pregistry), mask(pmask), all(pall) {}

            auto operator*() const -> C& {
                return *static_cast<C*>(&(registry->getAtIndex(index)));
            }

            auto operator==(const Iterator& other) const -> bool {
                return index == other.index;
            }
            auto operator!=(const Iterator& other) const -> bool {
                return index != other.index;
            }

            auto validIndex() -> bool {
                auto& entity = registry->getAtIndex(index);
                auto& amask  = entity.getComponentSignature();
                return (
                        all ||       // everything
                        (amask & mask) != 0// the correct component mask
                );
            }

            auto operator++() -> Iterator& {
                while (++index < registry->count() && !validIndex()) {}

                return *this;
            }

            size_t             index;
            EntityRegistry<C>* registry;
            ec_sig             mask;
            bool               all{false};
        };

        RegistryView(EntityRegistry<C>& registry)
            : m_registry(&registry) {
            if (sizeof...(ComponentTypes) == 0) {
                m_all = true;
            } else {
                // Unpack the template parameters into an initializer list
                component_id component_ids[] = {0, m_registry->template getComponentType<ComponentTypes>()...};
                for (int i = 1; i < static_cast<int>((sizeof...(ComponentTypes) + 1)); i++) {
                    m_component_mask.set(component_ids[i]);
                }
            }
        }

        auto begin() -> Iterator {
            int first_index = 0;
            while (first_index < static_cast<int>(m_registry->count())) {

                if (!m_registry->getAtIndex(static_cast<size_t>(first_index)).isActive()) {
                    return end();
                }

                if (m_all) {
                    break;
                }

                ec_sig entity_mask = m_registry->getAtIndex(static_cast<size_t>(first_index)).getComponentSignature();
                if ((m_component_mask & entity_mask) == 0) {
                    first_index++;
                } else {
                    break;
                }
            }
            return Iterator(m_registry, static_cast<size_t>(first_index), m_component_mask, m_all);
        }

        auto end() -> Iterator {
            return Iterator(m_registry, static_cast<size_t>(m_registry->count()), m_component_mask, m_all);
        }

    private:
        EntityRegistry<C>* m_registry;
        bool               m_all{false};
        ec_sig             m_component_mask;
    };

}// namespace rosa::ecs