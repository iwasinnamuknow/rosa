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

#include <cmath>
#include <core/SerialiserTypes.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace rosa {

    /**
     * \brief A component which handles the transform of an entity
     *
     * Every entity is created by default with a transform component. It is the
     * interface by which the entity can be repositioned, scaled and rotated.
     */
    struct TransformComponent {
        glm::vec3 position{0, 0, 0};
        glm::vec3 scale{1, 1, 1};
        float     rotation{0};
        glm::mat4 parent_transform{1.F};

        auto getLocalTransform() const -> const glm::mat4;
        auto getGlobalTransform() const -> const glm::mat4;

        auto setPosition(float x, float y) -> void {
            position = glm::vec3(x, y, 1.F);
        }

        auto setPosition(glm::vec2 pos) -> void {
            position = glm::vec3(pos, 1.F);
        }

        auto getPosition() const -> const glm::vec2 {
            return glm::vec2(position[0], position[1]);
        }

        auto setScale(glm::vec2 new_scale) -> void {
            scale = glm::vec3(new_scale, 1.F);
        }

        auto getScale() const -> const glm::vec2 {
            return {scale.x, scale.y};
        }

        auto setRotation(float rot) -> void {
            rotation = rot;
        }

        auto getRotation() const -> float {
            return rotation;
        }
    };

    auto operator<<(YAML::Emitter& out, const TransformComponent& component) -> YAML::Emitter&;

}// namespace rosa

namespace YAML {
    template<>
    struct convert<rosa::TransformComponent> {
        static auto decode(const Node& node, rosa::TransformComponent& rhs) -> bool {
            if (!node.IsMap()) {
                return false;
            }

            rhs.position = node["position"].as<glm::vec3>();
            rhs.scale    = node["scale"].as<glm::vec3>();
            rhs.rotation = node["rotation"].as<float>();
            return true;
        }

        static auto encode(const rosa::TransformComponent& rhs) -> Node {
            Node node;
            node["position"] = rhs.position;
            node["scale"]    = rhs.scale;
            node["rotation"] = rhs.rotation;
            return node;
        }
    };
}// namespace YAML