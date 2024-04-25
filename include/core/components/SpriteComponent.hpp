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
#include <core/ResourceManager.hpp>
#include <graphics/Sprite.hpp>

#include <string_view>

namespace rosa {

    class SceneSerialiser;

    struct SpriteComponent : Sprite {

        auto getTextureUuid() const -> const Uuid& {
            return getTexture().getUuid();
        }

        private:
            friend class SceneSerialiser;
    };

    auto operator<<(YAML::Emitter& out, const SpriteComponent& component) -> YAML::Emitter&;

} // namespace rosa

namespace YAML {
    template<>
    struct convert<rosa::SpriteComponent> {
        static auto decode(const Node& node, rosa::SpriteComponent& rhs) -> bool {
            if (!node.IsMap()) {
                return false;
            }

            rhs.setTexture(node["texture"].as<rosa::Uuid>());
            rhs.setColour(node["colour"].as<rosa::Colour>());
            return true;
        }

        static auto encode(const rosa::SpriteComponent& rhs) -> Node {
            Node node;
            node["texture"] = rhs.getTextureUuid().toString();
            node["colour"]  = rhs.getColour();
            return node;
        }
    };
}// namespace YAML