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
            Sprite() {
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

            // auto getBounds() -> Rect<float> {

            //     auto pos = getLocalTransform() * glm::vec4(1.F);

            //     if (m_texture == nullptr) {
            //         return {m_position[0], m_position[1], 0, 0};
            //     }
                
            //     return {pos[0], pos[1], m_texture->getSize().x * m_scale[0], m_texture->getSize().y * m_scale[1]};
            // }

            auto setTexture(Texture& texture) -> void {
                m_texture = &texture;
            }

            auto setSetShader(ShaderType type, Shader& shader) -> void {
                if (type == ShaderType::FragmentShader) {
                    m_fragment_shader = &shader;
                } else if (type == ShaderType::VertexShader) {
                    m_vertex_shader = &shader;
                }

                link_shaders();
            }

        protected:
            auto draw(glm::mat4 projection, glm::mat4 transform) -> void override;

        private:
            friend class Window;
            Vertex m_vertices[4];
            
            unsigned int m_pid;
            unsigned int m_vertex_buffer;
            unsigned int m_vertex_array;
            unsigned int m_index_buffer;

            Texture* m_texture{nullptr};
            Shader* m_vertex_shader{nullptr};
            Shader* m_fragment_shader{nullptr};

            glm::mat4 m_mvp{1.F};

            auto gl_init() -> void;

            unsigned int m_index[2*3]{
                0, 1, 2,
                1, 2, 3
            };

            int m_mvp_id;

            auto link_shaders() -> void;
    };

} // namespace rosa