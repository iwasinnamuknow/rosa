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

#include <core/input/Keyboard.hpp>
#include <core/input/Mouse.hpp>

namespace rosa {

    /**
     * \brief Associates an identifier with each of the main input types
     */
    enum EventType {
        EventKeyboard,
        EventMouse,
        EventResize,
        EventClose
    };

    /**
     * \brief When the window is resized, we pass the new size on as event information
     */
    struct ResizeEvent {
        glm::vec2 size; ///< Size of the window after resizing
    };

    /**
     * \brief Container for event information
     *
     * The member union will contain information for the relevant event.
     * Check the EventType to determine what information to access.
     */
    struct Event {
        EventType type; ///< Denotes the type of event

        /** This union will contain information for a single event type **/
        union {
            MouseEvent mouse;       ///< Mouse event information
            KeyboardEvent keyboard; ///< Keyboard event information
            ResizeEvent resize;     ///< Window resize information
        };
    };

} // namespace rosa