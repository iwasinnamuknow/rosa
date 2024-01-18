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

#include <graphics/gl.hpp>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

namespace rosa {

    enum MouseButton {
        MouseButtonLeft = GLFW_MOUSE_BUTTON_LEFT,
        MouseButtonRight = GLFW_MOUSE_BUTTON_RIGHT,
        MouseButtonMiddle = GLFW_MOUSE_BUTTON_MIDDLE
    };

    enum MouseEventType {
        MouseButtonPressed,
        MouseButtonReleased,
        MouseMoved,
        MouseScrolled
    };

    struct MouseEvent {
        MouseEventType type;

        union {
            MouseButton button;
            glm::vec2 position;
        };
    };

} // namespace rosa