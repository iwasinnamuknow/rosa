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

#include <yaml-cpp/yaml.h>
#include <glm/glm.hpp>

namespace rosa {
    auto operator<<(YAML::Emitter& out, const glm::vec2& vec) -> YAML::Emitter&;
    auto operator<<(YAML::Emitter& out, const glm::vec3& vec) -> YAML::Emitter&;
} // namespace rosa

namespace YAML {
    template<>
    struct convert<glm::vec2> {
        static auto decode(const Node& node, glm::vec2& rhs) -> bool {
            if (!node.IsSequence() || node.size() != 2) {
                return false;
            }

            rhs.x = node[0].as<float>();
            rhs.y = node[1].as<float>();
            return true;
        }

        static auto encode(const glm::vec2& rhs) -> Node {
            Node node;
            node.push_back(rhs.x);
            node.push_back(rhs.y);
            return node;
        }
    };

    template<>
    struct convert<glm::vec3> {
        static auto decode(const Node& node, glm::vec3& rhs) -> bool {
            if (!node.IsSequence() || node.size() != 3) {
                return false;
            }

            rhs.x = node[0].as<float>();
            rhs.y = node[1].as<float>();
            rhs.z = node[2].as<float>();
            return true;
        }

        static auto encode(const glm::vec3& rhs) -> Node {
            Node node;
            node.push_back(rhs.x);
            node.push_back(rhs.y);
            node.push_back(rhs.z);
            return node;
        }
    };
} // namespace YAML