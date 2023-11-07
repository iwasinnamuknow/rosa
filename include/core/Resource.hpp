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

#include <audio/AudioFile.hpp>
#include <soloud/include/soloud_wavstream.h>
#include <string>
#include <stduuid/uuid.h>

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
            Resource(std::string name, uuids::uuid uuid) : m_name(std::move(name)), m_uuid(uuid) {}
            virtual ~Resource() = default;

            auto getName() -> const std::string& {
                return m_name;
            }

            auto getUUID() -> const uuids::uuid& {
                return m_uuid;
            }

        private:
            std::string m_name{};
            uuids::uuid m_uuid{};

            virtual auto loadFromPhysFS() -> bool = 0;

            friend class ResourceManager;
    };

} // namespace rosa