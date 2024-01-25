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

#include "core/input/Mouse.hpp"
#include <GLFW/glfw3.h>
#include <core/EventManager.hpp>
#include <cstddef>
#include <cstdint>
#include <glm/gtc/matrix_transform.hpp>
#include <graphics/RenderWindow.hpp>
#include <iostream>
#include <tracy/TracyOpenGL.hpp>

void key_callback(GLFWwindow* /*window*/, int key, int /*scancode*/, int action, int mode) {
    rosa::KeyboardEvent kb_event{};
    kb_event.key       = static_cast<rosa::Key>(key);
    kb_event.type      = static_cast<rosa::KeyboardEventType>(action);
    kb_event.modifiers = static_cast<rosa::KeyboardModifier>(mode);

    rosa::Event event{};
    event.type     = rosa::EventType::EventKeyboard;
    event.keyboard = kb_event;
    rosa::EventManager::getInstance().pushEvent(event);
}

void cursor_pos_callback(GLFWwindow* /*window*/, double xpos, double ypos) {
    rosa::MouseEvent m_event{};
    m_event.type     = rosa::MouseEventType::MouseMoved;
    m_event.position = glm::vec2(xpos, ypos);

    rosa::Event event{};
    event.type  = rosa::EventType::EventMouse;
    event.mouse = m_event;
    rosa::EventManager::getInstance().pushEvent(event);
}

void mouse_button_callback(GLFWwindow* /*window*/, int button, int action, int /*mods*/) {
    rosa::MouseEvent m_event{};

    if (action == GLFW_PRESS) {
        m_event.type = rosa::MouseEventType::MouseButtonPressed;
    } else if (action == GLFW_RELEASE) {
        m_event.type = rosa::MouseEventType::MouseButtonReleased;
    }

    m_event.button = static_cast<rosa::MouseButton>(button);

    rosa::Event event{};
    event.type  = rosa::EventType::EventMouse;
    event.mouse = m_event;
    rosa::EventManager::getInstance().pushEvent(event);
}

namespace rosa {

    RenderWindow::RenderWindow(int width, int height, const std::string& title, int msaa, bool window_hidden) {
        glfwInit();

        spdlog::debug("RenderWindow: Attempting to create OpenGL context");

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        if (window_hidden) {
            glfwWindowHint(GLFW_VISIBLE, GL_FALSE);
        }

        m_wnd = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
        if (m_wnd == nullptr) {
            glfwTerminate();
            throw RenderException("Failed to create the OpenGL context");
        }

        glfwMakeContextCurrent(m_wnd);
        //glfwSwapInterval(0); // Vsync disable

        gladLoadGL(glfwGetProcAddress);

        // validate multi sample count
        int max_msaa = 8;
        glGetIntegerv(GL_MAX_SAMPLES, &max_msaa);
        if (msaa < 1) {
            msaa = 1;
        } else if (msaa > max_msaa) {
            msaa = max_msaa;
        } else if (msaa % 2 != 0) {
            msaa--;
        }

        spdlog::debug("RenderWindow: Multisampling at {}x", msaa);
        glfwWindowHint(GLFW_SAMPLES, msaa);

        TracyGpuContext;

        GLint version_major{};
        GLint version_minor{};
        glGetIntegerv(GL_MAJOR_VERSION, &version_major);
        glGetIntegerv(GL_MINOR_VERSION, &version_minor);

        auto vendor   = std::string(reinterpret_cast<const char*>(glGetString(GL_VENDOR)));
        auto renderer = std::string(reinterpret_cast<const char*>(glGetString(GL_RENDERER)));

        spdlog::info("RenderWindow: Context created with OpenGL {}.{} using {} ({})", version_major, version_minor, renderer, vendor);

        glfwSetKeyCallback(m_wnd, key_callback);
        glfwSetCursorPosCallback(m_wnd, cursor_pos_callback);
        glfwSetMouseButtonCallback(m_wnd, mouse_button_callback);

        glfwSetWindowUserPointer(m_wnd, this);
        glfwSetWindowSizeCallback(m_wnd, RenderWindow::callbackResize);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glEnable(GL_MULTISAMPLE);

        m_monitor = glfwGetPrimaryMonitor();
        glfwGetWindowSize(m_wnd, m_wnd_size.data(), &m_wnd_size[1]);
        glfwGetWindowPos(m_wnd, m_wnd_pos.data(), &m_wnd_pos[1]);
        m_update_viewport = true;

        m_view_matrix       = glm::lookAt(glm::vec3(0.F, 0.F, -1.F), glm::vec3(0.F, 0.F, 1.F), glm::vec3(0.F, 1.F, 0.F));
        m_projection_matrix = glm::ortho(0.F, static_cast<float>(m_vp_size[0]), static_cast<float>(m_vp_size[1]), 0.F);

        if (msaa > 1) {
            m_framebuffer.init(width, height, msaa);
        } else {
            m_framebuffer.init(width, height);
        }
    }

    void RenderWindow::callbackResize(GLFWwindow* window, int changed_x, int changed_y) {
        void* ptr = glfwGetWindowUserPointer(window);
        if (auto* wnd_ptr = static_cast<RenderWindow*>(ptr)) {
            wnd_ptr->resize(changed_x, changed_y);
        }
    }

    void RenderWindow::resize(int change_x, int change_y) {
        spdlog::debug("RenderWindow: Window resized. New size is {},{}", change_x, change_y);
        m_wnd_size[0] = change_x;
        m_wnd_size[1] = change_y;
        m_framebuffer.init(change_x, change_y);

        m_update_viewport = true;
        m_projection_matrix = glm::ortho(0.F, static_cast<float>(m_vp_size[0]), static_cast<float>(m_vp_size[1]), 0.F);

        rosa::Event event{};
        event.type   = rosa::EventType::EventResize;
        event.resize = {glm::vec2(change_x, change_y)};
        rosa::EventManager::getInstance().pushEvent(event);
    }

    auto RenderWindow::pollEvents() -> bool {
        glfwPollEvents();
        return false;
    }

    auto RenderWindow::isOpen() const -> bool {
        return glfwWindowShouldClose(m_wnd) == 0;
    }

    auto RenderWindow::display(glm::vec2 camera_pos) -> void {
        if (m_update_viewport) {
            TracyGpuZone("Update Viewport");
            glfwGetFramebufferSize(m_wnd, &m_vp_size[0], &m_vp_size[1]);
            m_update_viewport = false;
            glViewport(0, 0, m_vp_size[0], m_vp_size[1]);
        }

        m_view_matrix       = glm::lookAt(glm::vec3(camera_pos.x, camera_pos.y, 1.F), glm::vec3(camera_pos.x, camera_pos.y, 0.F), glm::vec3(0.F, 1.F, 0.F));
        m_projection_matrix = glm::ortho(0.F, static_cast<float>(m_vp_size[0]), static_cast<float>(m_vp_size[1]), 0.F);

        {
            TracyGpuZone("Swap Buffers");
            glfwSwapBuffers(m_wnd);
        }
    }

    auto RenderWindow::clearColour(Colour colour) -> void {
        ZoneScopedN("Render:Clear");
        TracyGpuZone("Clear");
        glClearColor(colour.r, colour.g, colour.b, colour.a);
        glClear(GL_COLOR_BUFFER_BIT);
    }

    auto RenderWindow::getViewportSize() const -> glm::vec2 {
        return {m_vp_size[0], m_vp_size[1]};
    }

    auto RenderWindow::isFullscreen() const -> bool {
        return glfwGetWindowMonitor(m_wnd) != nullptr;
    }

    void RenderWindow::setFullScreen(bool fullscreen) {
        if (isFullscreen() == fullscreen) {
            return;
        }

        if (fullscreen) {
            // backup window position and window size
            glfwGetWindowPos(m_wnd, m_wnd_pos.data(), &m_wnd_pos[1]);
            glfwGetWindowSize(m_wnd, m_wnd_size.data(), &m_wnd_size[1]);

            // get resolution of monitor
            const GLFWvidmode* mode = glfwGetVideoMode(m_monitor);

            // switch to full screen
            glfwSetWindowMonitor(m_wnd, m_monitor, 0, 0, mode->width, mode->height, 0);
        } else {
            // restore last window size and position
            glfwSetWindowMonitor(m_wnd, nullptr, m_wnd_pos[0], m_wnd_pos[1], m_wnd_size[0], m_wnd_size[1], 0);
        }

        m_update_viewport = true;
    }

    auto RenderWindow::getProjection() const -> glm::mat4 {
        return m_projection_matrix;
    }

    auto RenderWindow::getView() const -> glm::mat4 {
        return m_view_matrix;
    }

    auto RenderWindow::isKeyDown(Key key) -> bool {
        return glfwGetKey(m_wnd, key);
    }

    auto RenderWindow::readFrame() -> std::span<unsigned char> {
        return m_framebuffer.getColorBuffer();
    }

    auto RenderWindow::getFrameBuffer() -> FrameBuffer& {
        return m_framebuffer;
    }

}// namespace rosa
