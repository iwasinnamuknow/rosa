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

    class GameManager;

    /**
     * \brief An error occurred in the render context
     */
    class RenderException : public Exception {
    public:
        explicit RenderException(const std::string& msg)
            : Exception(msg) {
        }
    };

    /**
     * \brief Provides and interface to the OpenGL context
     *
     * The RenderWindow handles initialising the OpenGL context and window. It also polls for events
     * and pushes them to the event queue. It can handle resizing the window and going into/out of
     * fullscreen.
     */
    class RenderWindow {
    public:
        /**
         * \brief Create the render window
         * \param width window width
         * \param height window height
         * \param title window title
         * \param msaa MSAA multiplier
         * \param window_hidden Make the window invisible
         */
        RenderWindow(int width, int height, const std::string& title = "Rosa Engine", int msaa = 1, bool window_hidden = false);

        /**
         * \brief Tracks whether window closure has been requested
         */
        auto isOpen() const -> bool;

        /**
         * \brief Request window closure
         */
        auto close() -> void {
            glfwSetWindowShouldClose(m_wnd, 1);
        }

        /**
         * \brief Clear the window with a background colour
         */
        auto clearWindow(Colour colour) -> void;

        /**
         * \brief Check if the window is fullscreen
         */
        auto isFullscreen() const -> bool;

        /**
         * \brief Set the fullscreen state of the window
         */
        auto setFullScreen(bool fullscreen) -> void;

        /**
         * \brief Get the pixel size of the window
         */
        auto getWindowSize() const -> glm::vec2 {
            return {m_wnd_size[0], m_wnd_size[1]};
        }

        /**
         * \brief Discover if a key is being pressed
         */
        auto isKeyDown(Key key) -> bool;

        /**
         * \brief Get the projection matrix
         */
        auto getProjection() const -> glm::mat4;

        /**
         * \brief Get the view matrix
         */
        auto getView() const -> glm::mat4;

        /**
         * \brief Get the framebuffer
         */
        auto getFrameBuffer() -> FrameBuffer&;

        /**
         * \brief Poll for input events and push to the queue
         */
        auto pollEvents() -> void;

        /**
         * \brief Get the size of the OpenGL viewport
         */
        auto getViewportSize() const -> glm::vec2;

        /**
         * \brief Update the window, viewport and calculate matrices
         * \param camera_pos World-space position of the active camera
         */
        auto display(glm::vec2 camera_pos) -> void;

        /**
         * \brief Get pixel data for the current framebuffer state
         */
        auto readFrame() -> std::span<unsigned char>;

        /**
         * \brief This callback is used by OpenGL on resizing
         */
        static void callbackResize(GLFWwindow* window, int changed_x, int changed_y);

    private:
        std::array<int, 2> m_wnd_pos{0, 0};
        std::array<int, 2> m_wnd_size{0, 0};
        std::array<int, 2> m_vp_size{0, 0};
        bool               m_update_viewport = true;
        GLFWwindow*        m_wnd             = nullptr;
        GLFWmonitor*       m_monitor         = nullptr;

        void resize(int change_x, int change_y);

        /**
         * \brief Get an OpenGL window pointer
         */
        auto getGlWindowPtr() -> GLFWwindow* {
            return m_wnd;
        }

        glm::mat4 m_projection_matrix{};
        glm::mat4 m_view_matrix{0.F};

        FrameBuffer m_framebuffer;

        friend class GameManager;
    };

}// namespace rosa