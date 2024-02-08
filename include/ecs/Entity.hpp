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

#include <bitset>
#include <core/Uuid.hpp>
#include <ecs/Component.hpp>
#include <memory>

namespace rosa::ecs {

    constexpr std::uint32_t max_entities{50000};
    using ec_sig = std::bitset<ecs::max_components>;

    class Entity {
    public:
        Entity();
        explicit Entity(const rosa::Uuid& uuid);

        // copy assignment
        auto operator=(const Entity& other) -> Entity& = default;
        // copy constructor
        Entity(const Entity& other) = default;

        // move assigment
        auto operator=(Entity&& other) -> Entity& = default;
        // move constructor
        Entity(Entity&& other) noexcept = default;

        virtual ~Entity();

        [[nodiscard]] auto getUuid() const -> const rosa::Uuid&;

        [[nodiscard]] auto getComponentSignature() -> ec_sig&;

        operator Uuid() const {
            return getUuid();
        }

        virtual auto isActive() const -> bool {
            return false;
        }

    private:
        rosa::Uuid m_uuid{};
        ec_sig     m_component_sig{0};
    };

}// namespace rosa::ecs
