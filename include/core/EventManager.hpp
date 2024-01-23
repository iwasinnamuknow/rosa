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

#include "graphics/RenderWindow.hpp"
#include <queue>
#include <core/Event.hpp>

namespace rosa {

    /**
     * \brief Global handler for events
     *
     * Events are pushed into a queue when generated. The GameManager will pass events onto Lua or
     * NativeScript onInput.
     */
    class EventManager {
    public:
        EventManager(const EventManager&) = delete;
        auto operator=(const EventManager&) -> EventManager& = delete;

        EventManager(EventManager&&) = delete;
        auto operator=(EventManager&&) -> EventManager& = delete;

        /**
         * \brief Get a reference to the global EventManager
         */
        static auto getInstance() -> EventManager& {
            static EventManager instance;
            return instance;
        }

        /**
         * \brief Push an event onto the queue
         * \param event a new event
         */
        auto pushEvent(Event event) -> void {
            m_events.push(event);
        }

        /**
         * \brief Pop the first event from the queue
         * \return the first event
         */
        auto popEvent() -> Event {
            auto event = m_events.front();
            m_events.pop();
            return event;
        }

        /**
         * \brief Check whether the queue has events
         * \return false if empty, true if not
         */
        auto hasEvents() -> bool {
            return !m_events.empty();
        }

        /**
         * \brief Tell the RenderWindow to poll for events
         * \param window the render window
         */
        auto pollEvents(RenderWindow& window) -> void {
            window.pollEvents();
        }

    private:
        EventManager() = default;
        ~EventManager() = default;

        std::queue<Event> m_events{};
    };

} // namespace rosa