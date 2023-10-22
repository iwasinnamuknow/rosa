/*
 * This file is part of glad.
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

#include "glm/ext/vector_float4.hpp"
#include <graphics/Vertex.hpp>
#include <graphics/Texture.hpp>
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
            Sprite(glm::vec2 pos = glm::vec2(0.F, 0.F)) : m_position(glm::vec4(pos.x, pos.y, 1.F, 1.F)) {
                gl_init();
            }

            auto setColour(const Colour& colour) -> void {
                m_vertices[0].colour = colour;
                m_vertices[1].colour = colour;
                m_vertices[2].colour = colour;
                m_vertices[3].colour = colour;
            }

            auto getColour() const -> const Colour& {
                return m_vertices[0].colour;
            }

            // auto setPosition(Vector2<float> m_position) -> void {
            //     glm::translate(transform, m_position);
            // }

            auto setPosition(float x, float y) -> void {
                m_position = glm::vec3(x, y, 1.F);
            }

            auto setScale(float x, float y) -> void {
                m_scale = glm::vec3(x, y, 1.F);
            }

            auto getBounds() -> Rect<float> {

                auto pos = getLocalTransform() * glm::vec4(1.F);

                if (m_texture == nullptr) {
                    return {m_position[0], m_position[1], 0, 0};
                }
                
                return {pos[0], pos[1], m_texture->getSize().x * m_scale[0], m_texture->getSize().y * m_scale[1]};
            }

            auto setTexture(Texture& texture) -> void {
                m_texture = &texture;
            }

            auto getLocalTransform() const -> const glm::mat4;

        protected:
            auto draw(glm::mat4 projection) -> void override;

        private:
            friend class Window;
            Vertex m_vertices[4];
            auto update_transform(glm::mat4 projection) -> void;
            
            unsigned int m_pid;
            unsigned int m_vertex_buffer;
            unsigned int m_vertex_array;
            unsigned int m_index_buffer;

            Texture* m_texture{nullptr};

            auto gl_init() -> void;
            auto load_shaders(const char * vertex_file_path,const char * fragment_file_path) -> unsigned int;

            unsigned int m_index[2*3]{
                0, 1, 2,
                1, 2, 3
            };

            unsigned int m_mvp_id;

            glm::mat4 m_mvp;
            glm::vec3 m_position{1.F};
            glm::vec3 m_scale{1.F};
            glm::vec3 m_rotation{1.F};
    };

} // namespace rosa