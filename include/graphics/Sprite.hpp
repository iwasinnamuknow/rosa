/*
 * This file is part of rosa.
 *
 *  glad is free software: you can redistribute it and/or modify it under the terms of the
 *  GNU General Public License as published by the Free Software Foundation, either version
 *  3 of the License, or (at your option) any later version.
 *
 *  glad is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without
 *  even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along with bbai. If not,
 *  see <https://www.gnu.org/licenses/>.
 */

#pragma once

#include <glm/ext/vector_float4.hpp>
#include <graphics/Texture.hpp>
#include <graphics/Shader.hpp>
#include <graphics/Rect.hpp>
#include <graphics/Drawable.hpp>
#include <cstddef>
#include <sstream>
#include <fstream>
#include <vector>
#include <glm/gtc/matrix_transform.hpp>

class Window;

namespace rosa {

    class Sprite : public Drawable {

        public:
            Sprite();

            auto setColour(const Colour& colour) -> void {
                m_vertices[0].colour = colour;
                m_vertices[1].colour = colour;
                m_vertices[2].colour = colour;
                m_vertices[3].colour = colour;
            }

            auto getColour() const -> const Colour& {
                return m_vertices[0].colour;
            }

            auto setTexture(Texture& texture) -> void {
                m_texture = &texture;
            }

        protected:
            auto draw(glm::mat4 projection, glm::mat4 transform) -> void override;

        private:
            friend class Window;
            friend class NativeScriptEntity;

            Texture* m_texture{nullptr};
    };

} // namespace rosa