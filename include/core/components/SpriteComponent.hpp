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

#include <SFML/Graphics/Color.hpp>
#include <core/ResourceManager.hpp>
#include <SFML/Graphics.hpp>
#include <stduuid/uuid.h>
#include <string_view>

namespace rosa {

    class SceneSerialiser;

    struct SpriteComponent : sf::Sprite {
        
        auto getTexture() const -> uuids::uuid {
            return m_texture_uuid;
        }

        auto setTexture(uuids::uuid uuid) -> void {
            m_texture_uuid = uuid;
            sf::Sprite::setTexture(ResourceManager::instance().getTexture(m_texture_uuid));
        }

        private:
            uuids::uuid m_texture_uuid;
            //sf::Color m_color{255, 255, 255, 255};

            friend class SceneSerialiser;
    };

} // namespace rosa