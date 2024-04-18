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

#include <GLFW/glfw3.h>
#include <core/ResourceManager.hpp>
#include <cstddef>
#include <cstring>
#include <glm/glm.hpp>
#include <graphics/Renderer.hpp>
#include <graphics/Sprite.hpp>
#include <graphics/gl.hpp>
#include <spdlog/spdlog.h>

namespace rosa {

    Sprite::Sprite() {
    }

    auto Sprite::draw(glm::mat4 transform) -> void {

        if (m_shader_program == nullptr) {
            m_shader_program = Renderer::getInstance().makeShaderProgram();
        }

        //auto temppos = glm::vec4(m_quad.pos, 0, 0);
        //auto temptrans = (projection * transform);
        m_quad.pos.x = transform[3].x;
        m_quad.pos.y = transform[3].y;

        Renderable renderable{
                m_quad,
                transform,
                m_shader_program,
                m_screen_space};

        Renderer::getInstance().submit(renderable);
    }

    auto Sprite::setColour(const Colour& colour) -> void {
        m_quad.colour = colour;
    }

    auto Sprite::setTexture(const Uuid& uuid) -> void {
        m_texture = &ResourceManager::getInstance().getAsset<Texture>(uuid);
        m_quad.texture_id = m_texture->getOpenGlId();
        m_quad.texture_rect_size = glm::vec2(1, 1);
        m_quad.size = m_texture->getSize();
    }

    auto Sprite::setScreenSpace(bool screen_space) -> void {
        m_screen_space = screen_space;
    }

    auto Sprite::getScreenSpace() -> bool {
        return m_screen_space;
    }

    auto Sprite::getTexture() const -> Texture &
    {
        return *m_texture;
    }

    auto Sprite::getColour() const -> const Colour& {
        return m_quad.colour;
    }

    auto Sprite::setTextureRect(glm::vec2 position, glm::vec2 size) -> void {
        m_quad.texture_rect_pos = position / m_texture->getSize();
        m_quad.texture_rect_size = size / m_texture->getSize();
        m_quad.size = size;
    }

    auto Sprite::setTextureRect(Rect rect) -> void {
        setTextureRect(rect.position, rect.size);
    }

    auto Sprite::getSize() -> glm::vec2 {
        return m_quad.size;
    }

    auto Sprite::getTextureRect() -> Rect {
        return {m_quad.texture_rect_pos, m_quad.texture_rect_size};
    }


} // namespace rosa