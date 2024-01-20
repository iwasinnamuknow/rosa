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
#include <string>

namespace rosa {

    class ResourceManager;

    enum resource_type {
        resource_texture         = 0,
        resource_sound           = 1,
        resource_music           = 2,
        resource_font            = 3,
        resource_script          = 4,
        resource_vertex_shader   = 5,
        resource_fragment_shader = 6
    };

    class Resource {
    public:
        Resource(std::string name, Uuid uuid, std::string pack)
            : m_name(std::move(name)), m_uuid(uuid), m_pack(std::move(pack)) {}

        virtual ~Resource() = default;

        Resource(const Resource&)                    = delete;
        auto operator=(const Resource&) -> Resource& = delete;

        Resource(Resource&&)                    = delete;
        auto operator=(Resource&&) -> Resource& = delete;

        auto getName() -> const std::string& {
            return m_name;
        }

        auto getUUID() -> const Uuid& {
            return m_uuid;
        }

        auto getPack() -> const std::string& {
            return m_pack;
        }

    private:
        std::string m_name{};
        Uuid        m_uuid{};

        virtual auto loadFromPhysFS() -> void = 0;
        virtual auto closeHandles() -> void {
        }

        std::string m_pack{};

        friend class ResourceManager;
    };

}// namespace rosa