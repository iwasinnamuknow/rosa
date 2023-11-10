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

#include <core/ResourceManager.hpp>
#include <stduuid/uuid.h>
#include <string_view>
#include <graphics/Sprite.hpp>

namespace rosa {

    class SceneSerialiser;

    struct SpriteComponent : Sprite {

        auto getTextureUUID() -> uuids::uuid {
            return getTexture().getUUID();
        }

        auto setTexture(const std::string& uuid_str) -> void {
            auto uuid = uuids::uuid::from_string(uuid_str);
            if (uuid.has_value()) {
                Sprite::setTexture(uuid.value());
            } else {
                spdlog::error("Couldn't parse UUID in call to setTexture");
            }
        }

        private:
            friend class SceneSerialiser;
    };

} // namespace rosa