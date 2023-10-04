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

#include "core/PhysFSStream.hpp"
#include <string>
#include <stduuid/uuid.h>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

namespace rosa {

    class ResourceManager;

    enum resource_type {
        resource_texture = 0,
        resource_sound   = 1,
        resource_music   = 2,
        resource_font    = 3
    };

    class Resource {
        public:
            Resource(std::string name, uuids::uuid uuid, resource_type type) : m_name(std::move(name)), m_uuid(uuid), m_type(type) {}

            auto getName() -> const std::string& {
                return m_name;
            }

            auto getUUID() -> const uuids::uuid& {
                return m_uuid;
            }

            auto getType() -> resource_type {
                return m_type;
            }
        private:
            std::string m_name{};
            uuids::uuid m_uuid{};
            resource_type m_type{};

            sf::Texture m_texture;
            sf::Font m_font;
            sf::SoundBuffer m_sound_buffer;
            sf::Music m_music;
            PhysFsStream m_stream;

            bool m_loaded{false};

            friend class ResourceManager;
    };

} // namespace rosa