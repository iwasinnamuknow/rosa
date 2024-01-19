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

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <fmt/format.h>
#include <graphics/gl.hpp>
#include <GLFW/glfw3.h>
#include <array>
#include <stdexcept>
#include <vector>
#include <graphics/Sprite.hpp>
#include <graphics/Texture.hpp>
#include <core/input/Keyboard.hpp>
#include <graphics/FrameBuffer.hpp>

namespace rosa {

    class RenderWindow
    {
        private:

            std::array<int, 2> m_wndPos         {0, 0};
            std::array<int, 2> m_wndSize        {0, 0};
            std::array<int, 2> m_vpSize         {0, 0};
            bool                 m_updateViewport = true;
            GLFWwindow *         m_wnd            = nullptr;
            GLFWmonitor *        m_monitor        = nullptr;

            void resize( int cx, int cy );

            glm::mat4 m_projection;

            FrameBuffer m_framebuffer;

        public:
            void init( int width, int height, std::string title = "OpenGL", int msaa = 0, bool window_hidden = false);
            static void callback_resize(GLFWwindow* window, int cx, int cy);
            //auto draw(Drawable& drawable, glm::mat4 transform) -> void;
            auto isFullscreen() const -> bool;
            auto setFullScreen(bool fullscreen) -> void;
            auto clearColour(Colour colour) -> void;
            auto isOpen() const -> bool;
            auto display(glm::vec2 camera_pos) -> void;
            auto getSize() const -> glm::vec2 {
                return {m_wndSize[0], m_wndSize[1]};
            }

            auto getGlWindowPtr() -> GLFWwindow* {
                return m_wnd;
            }

            auto close() -> void {
                glfwSetWindowShouldClose(m_wnd, 1);
            }

            auto isKeyDown(Key key) -> bool;

        auto readFrame() -> std::span<const unsigned char>;

            auto getViewportSize() const -> glm::vec2;

            auto getProjection() const -> glm::mat4;

            auto getFrameBuffer() -> FrameBuffer&;

            // TODO EVENTS
            auto pollEvents() -> bool;
    };

} // namespace rosa