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

#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace rosa {

    struct TransformComponent {
        glm::vec3 position{0, 0, 0};
        glm::vec3 scale{1, 1, 1};
        //float rotation{0};

        glm::mat4 mvp;

        auto getLocalTransform() const -> const glm::mat4 {
            glm::mat4 translation_matrix = glm::translate(glm::mat4(1.F), position);
            glm::mat4 scale_matrix = glm::scale(glm::mat4(1.F), scale);
            return translation_matrix * scale_matrix;
        }

        auto setPosition(float x, float y) -> void {
            position = glm::vec3(x, y, 1.F);
        }

        auto getPosition() const -> const glm::vec2 {
            return glm::vec2(position[0], position[1]);
        }

        auto setScale(float x, float y) -> void {
            scale = glm::vec3(x, y, 1.F);
        }

        auto getScale() const -> const glm::vec2 {
            return glm::vec2(scale[0], scale[1]);
        }
    };

} // namespace rosa