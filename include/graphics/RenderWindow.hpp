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

#include <GLFW/glfw3.h>
#include <array>
#include <core/input/Keyboard.hpp>
#include <fmt/format.h>
#include <graphics/FrameBuffer.hpp>
#include <graphics/Sprite.hpp>
#include <graphics/Texture.hpp>
#include <graphics/gl.hpp>
#include <stdexcept>
#include <vector>

namespace rosa {

    /**
     * \brief An error occurred in the render context
     */
    class RenderException : public Exception {
    public:
        explicit RenderException(const std::string& msg)
            : Exception(msg) {
        }
    };

    class RenderWindow {
    public:
        RenderWindow(int width, int height, const std::string& title = "OpenGL", int msaa = 1, bool window_hidden = false);
        static void callbackResize(GLFWwindow* window, int changed_x, int changed_y);
        auto        isFullscreen() const -> bool;
        auto        setFullScreen(bool fullscreen) -> void;
        auto        clearColour(Colour colour) -> void;
        auto        isOpen() const -> bool;
        auto        display(glm::vec2 camera_pos) -> void;

        auto getSize() const -> glm::vec2 {
            return {m_wnd_size[0], m_wnd_size[1]};
        }

        auto getGlWindowPtr() -> GLFWwindow* {
            return m_wnd;
        }

        auto close() -> void {
            glfwSetWindowShouldClose(m_wnd, 1);
        }

        auto isKeyDown(Key key) -> bool;
        auto readFrame() -> std::span<unsigned char>;
        auto getViewportSize() const -> glm::vec2;
        auto getProjection() const -> glm::mat4;
        auto getFrameBuffer() -> FrameBuffer&;
        auto pollEvents() -> bool;

    private:
        std::array<int, 2> m_wnd_pos{0, 0};
        std::array<int, 2> m_wnd_size{0, 0};
        std::array<int, 2> m_vp_size{0, 0};
        bool               m_update_viewport = true;
        GLFWwindow*        m_wnd             = nullptr;
        GLFWmonitor*       m_monitor         = nullptr;

        void resize(int change_x, int change_y);

        glm::mat4 m_projection{};

        FrameBuffer m_framebuffer;
    };

}// namespace rosa