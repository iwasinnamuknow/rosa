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

#include <graphics/Texture.hpp>
#include <graphics/Drawable.hpp>
#include <graphics/Quad.hpp>
#include <graphics/Rect.hpp>
#include <core/Uuid.hpp>
#include <cstddef>

class Window;

namespace rosa {

    class Sprite : public Drawable {

        public:
            Sprite();

            auto setColour(const Colour& colour) -> void;
            auto getColour() const -> const Colour&;

            auto setTextureRect(glm::vec2 position, glm::vec2 size) -> void;
            auto setTextureRect(Rect rect) -> void;
            auto getTextureRect() -> Rect;

            auto getSize() -> glm::vec2;

            auto getTexture() -> Texture&;
            auto setTexture(Uuid uuid) -> void;

        protected:
            auto draw(glm::mat4 transform) -> void override;
            Texture* m_texture{nullptr};

        private:
            friend class Window;
            friend class NativeScriptEntity;
            friend class Scene;

            Quad m_quad;
    };

} // namespace rosa