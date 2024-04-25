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

#include <core/components/TransformComponent.hpp>

namespace rosa {

    auto TransformComponent::getLocalTransform() const -> const glm::mat4 {
        glm::mat4 translation_matrix = glm::translate(glm::mat4(1.F), position);
        glm::mat4 scale_matrix = glm::scale(glm::mat4(1.F), scale);
        glm::mat4 rotate_matrix = glm::rotate(glm::mat4(1.F), rotation, glm::vec3(0.F, 0.F, 1.F));
        return translation_matrix * rotate_matrix * scale_matrix;
    }

    auto TransformComponent::getGlobalTransform() const -> const glm::mat4 {
        glm::mat4 temp = parent_transform * getLocalTransform();
        temp[3][2] = 0;
        return temp;
    }

    auto operator<<(YAML::Emitter& out, const TransformComponent& component) -> YAML::Emitter& {
        out << YAML::BeginMap;
        out << YAML::Key << "type" << YAML::Value << "transform";
        out << YAML::Key << "position" << YAML::Value << component.position;
        out << YAML::Key << "scale" << YAML::Value << component.scale;
        out << YAML::Key << "rotation" << YAML::Value << component.rotation;
        out << YAML::EndMap;
        return out;
    }

} // namespace rosa