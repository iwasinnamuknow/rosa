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

#include <core/components/SpriteComponent.hpp>
#include <yaml-cpp/yaml.h>

namespace rosa {

    auto operator<<(YAML::Emitter& out, const SpriteComponent& component) -> YAML::Emitter& {
        out << YAML::BeginMap;
        out << YAML::Key << "type" << YAML::Value << "sprite";
        out << YAML::Key << "texture" << YAML::Value << static_cast<std::string>(component.getTextureUuid());
        out << YAML::Key << "colour" << YAML::Value << component.getColour();
        out << YAML::EndMap;
        return out;
    }

}// namespace rosa
