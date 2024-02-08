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

#include <ecs/Entity.hpp>

namespace rosa::ecs {

    Entity::Entity()
        : m_uuid(rosa::Uuid::generate()) {
    }

    Entity::Entity(const rosa::Uuid& uuid)
        : m_uuid(uuid) {
    }

    Entity::~Entity() = default;

    auto Entity::getUuid() const -> const rosa::Uuid& {
        return m_uuid;
    }

    auto Entity::getComponentSignature() -> ec_sig& {
        return m_component_sig;
    }

}// namespace rosa::ecs