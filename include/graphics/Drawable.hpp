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

#include <graphics/RenderWindow.hpp>
#include <graphics/Vertex.hpp>
#include <graphics/Quad.hpp>
#include <graphics/Shader.hpp>
#include <glm/glm.hpp>

namespace rosa {

    class Drawable {
        public:
            friend class RenderWindow;
            Drawable() {
                
            }
            virtual ~Drawable() = default;
            virtual auto draw(glm::mat4 transform) -> void{}

        protected:
            friend class RenderWindow;

            //Quad m_quad;

            
    };

} // namespace rosa