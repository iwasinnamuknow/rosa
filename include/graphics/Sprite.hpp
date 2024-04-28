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
 *  You should have received a copy of the GNU General Public License along with rosa. If not,
 *  see <https://www.gnu.org/licenses/>.
 */

#pragma once

#include <core/Uuid.hpp>
#include <cstddef>
#include <graphics/Drawable.hpp>
#include <graphics/Quad.hpp>
#include <graphics/Rect.hpp>
#include <graphics/ShaderProgram.hpp>
#include <graphics/Texture.hpp>

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

            auto getTexture() const -> Texture&;
            auto setTexture(const Uuid& uuid) -> void;

            auto setScreenSpace(bool screen_space) -> void;
            auto getScreenSpace() -> bool;

            auto setShaders(const Uuid& vertex, const Uuid& fragment) -> void;
            auto getVertexShader() -> const Uuid&;
            auto getFragmentShader() -> const Uuid&;

        protected:
            auto draw(glm::mat4 transform) -> void override;
            Texture* m_texture{nullptr};

        private:
            friend class Window;
            friend class NativeScriptEntity;
            friend class Scene;

            Quad m_quad;
            bool m_screen_space{false};

            rosa::Uuid     m_vertex_shader{"00000000-0000-0000-0000-000000000001"};
            rosa::Uuid     m_fragment_shader{"00000000-0000-0000-0000-000000000002"};
            ShaderProgram* m_shader_program{nullptr};
    };

} // namespace rosa