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

#include "core/input/Mouse.hpp"
#include <GLFW/glfw3.h>
#include <graphics/RenderWindow.hpp>
#include <cstddef>
#include <cstdint>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <core/EventManager.hpp>
#include <tracy/TracyOpenGL.hpp>

void key_callback(GLFWwindow* /*window*/, int key, int /*scancode*/, int action, int mode) {
    // TODO EVENTS
    //if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    //    glfwSetWindowShouldClose(window, GL_TRUE);

    rosa::KeyboardEvent kb_event{};
    kb_event.key = static_cast<rosa::Key>(key);
    kb_event.type = static_cast<rosa::KeyboardEventType>(action);
    kb_event.modifiers = static_cast<rosa::KeyboardModifier>(mode);

    rosa::Event event{};
    event.type = rosa::EventType::EventKeyboard;
    event.keyboard = kb_event;
    rosa::EventManager::getInstance().pushEvent(event);
}

void cursor_pos_callback(GLFWwindow* /*window*/, double xpos, double ypos) {
    rosa::MouseEvent m_event{};
    m_event.type = rosa::MouseEventType::MouseMoved;
    m_event.position = glm::vec2(xpos, ypos);

    rosa::Event event{};
    event.type = rosa::EventType::EventMouse;
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
    event.type = rosa::EventType::EventMouse;
    event.mouse = m_event;
    rosa::EventManager::getInstance().pushEvent(event);
}

namespace rosa {

    void RenderWindow::init( int width, int height, std::string title)
    {
        glfwInit();

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_SAMPLES, 4);
        //glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);

        m_wnd = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
        if (m_wnd == nullptr)
        {
            glfwTerminate();
            throw std::runtime_error( "error initializing window" ); 
        }

        glfwMakeContextCurrent( m_wnd );
        //glfwSwapInterval(0); // Vsync disable

        gladLoadGL(glfwGetProcAddress);

        TracyGpuContext;

        GLint version_major{};
        GLint version_minor{};
        glGetIntegerv(GL_MAJOR_VERSION, &version_major);
        glGetIntegerv(GL_MINOR_VERSION, &version_minor);

        auto vendor = std::string(reinterpret_cast<const char*>(glGetString(GL_VENDOR)));
        auto renderer = std::string(reinterpret_cast<const char*>(glGetString(GL_RENDERER)));

        spdlog::info("Context created with OpenGL {}.{} using {} ({})", version_major, version_minor, renderer, vendor);

        glfwSetKeyCallback(m_wnd, key_callback);
        glfwSetCursorPosCallback(m_wnd, cursor_pos_callback);
        glfwSetMouseButtonCallback(m_wnd, mouse_button_callback);

        glfwSetWindowUserPointer( m_wnd, this );
        glfwSetWindowSizeCallback( m_wnd, RenderWindow::callback_resize );

        m_monitor =  glfwGetPrimaryMonitor();
        glfwGetWindowSize( m_wnd, m_wndSize.data(), &m_wndSize[1] );
        glfwGetWindowPos( m_wnd, m_wndPos.data(), &m_wndPos[1] );
        m_updateViewport = true;

        m_projection = glm::ortho(0.F, static_cast<float>(width), static_cast<float>(height), 0.F);

        m_framebuffer.init(m_wndSize[0], m_wndSize[1]);
    }

    void RenderWindow::callback_resize(GLFWwindow* window, int cx, int cy)
    {
        void *ptr = glfwGetWindowUserPointer( window );
        if (auto *wnd_ptr = static_cast<RenderWindow*>(ptr)) {
            wnd_ptr->resize(cx, cy );
        }
    }

    void RenderWindow::resize( int cx, int cy )
    {
        m_updateViewport = true;
        m_projection = glm::ortho(0.F, static_cast<float>(cx), static_cast<float>(cy), 0.F);

        rosa::Event event{};
        event.type = rosa::EventType::EventResize;
        event.resize = {glm::vec2(cx, cy)};
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
        if ( m_updateViewport )
        {
            TracyGpuZone("Update Viewport");
            glfwGetFramebufferSize( m_wnd, &m_vpSize[0], &m_vpSize[1] );
            m_updateViewport = false;
        }

        glViewport(camera_pos.x, camera_pos.y, m_vpSize[0], m_vpSize[1]);
        m_projection = glm::ortho(0.F, static_cast<float>(m_vpSize[0]), static_cast<float>(m_vpSize[1]), 0.F);

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

    // auto RenderWindow::draw(Drawable& drawable, glm::mat4 transform) -> void {
    //     drawable.m_mvp = m_projection * transform;
    //     glUseProgram(drawable.m_pid);
    //     glUniformMatrix4fv(drawable.m_mvp_id, 1, GL_FALSE, &drawable.m_mvp[0][0]);
    //     drawable.draw(m_projection, transform);
    //     glUseProgram(0);
    // }

    auto RenderWindow::getViewportSize() const -> glm::vec2 {
        return {m_vpSize[0], m_vpSize[1]};
    }

    auto RenderWindow::isFullscreen() const -> bool
    {
        return glfwGetWindowMonitor( m_wnd ) != nullptr;
    } 

    void RenderWindow::setFullScreen( bool fullscreen )
    {
        if (isFullscreen() == fullscreen) {
            return;
        }

        if (fullscreen)
        {
            // backup window position and window size
            glfwGetWindowPos( m_wnd, m_wndPos.data(), &m_wndPos[1] );
            glfwGetWindowSize( m_wnd, m_wndSize.data(), &m_wndSize[1] );
            
            // get resolution of monitor
            const GLFWvidmode * mode = glfwGetVideoMode(m_monitor);

            // switch to full screen
            glfwSetWindowMonitor( m_wnd, m_monitor, 0, 0, mode->width, mode->height, 0 );
        }
        else
        {
            // restore last window size and position
            glfwSetWindowMonitor( m_wnd, nullptr,  m_wndPos[0], m_wndPos[1], m_wndSize[0], m_wndSize[1], 0 );
        }

        m_updateViewport = true;
    }

    auto RenderWindow::getProjection() const -> glm::mat4 {
        return m_projection;
    }

    auto RenderWindow::isKeyDown(Key key) -> bool {
        return glfwGetKey(m_wnd, key);
    }

    auto RenderWindow::readFrame() -> std::vector<unsigned char> {

        int size = m_wndSize[0] * m_wndSize[1] * 4 * sizeof(unsigned char);
        const unsigned char* buffer = m_framebuffer.getColorBuffer();

        std::vector<unsigned char> data{ buffer, buffer + size };

        return data;
    }

    auto RenderWindow::getFrameBuffer() -> FrameBuffer& {
        return m_framebuffer;
    }

} // namespace rosa
