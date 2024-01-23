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

/*! \file */

#pragma once

#include <graphics/gl.hpp>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

namespace rosa {

    /**
     * \brief Associates an identifier with the three primary mouse buttons
     */
    enum MouseButton {
        MouseButtonLeft = GLFW_MOUSE_BUTTON_LEFT,
        MouseButtonRight = GLFW_MOUSE_BUTTON_RIGHT,
        MouseButtonMiddle = GLFW_MOUSE_BUTTON_MIDDLE
    };

    /**
     * \brief Denotes the type of mouse event
     */
    enum MouseEventType {
        MouseButtonPressed,     ///< A mouse button was pressed down
        MouseButtonReleased,    ///< A mouse button was released
        MouseMoved,             ///< The mouse cursor was moved
        MouseScrolled           ///< The mouse wheel was scrolled
    };

    /**
     * \brief Contains information about the mouse event
     *
     * The union will contain information about the event, check the
     * MouseEventType to determine what part of the union to access.
     */
    struct MouseEvent {
        /** The type of mouse event **/
        MouseEventType type;

        /** This union will contain information for a single event type **/
        union {
            MouseButton button; ///< Mouse button for MouseButtonPressed and MouseButtonReleased
            glm::vec2 position; ///< Position for cursor movement
        };
    };

} // namespace rosa