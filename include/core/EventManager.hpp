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

    class EventManager {
        public:
            static auto getInstance() -> EventManager& {
                static EventManager instance;
                return instance;
            }
            
            auto pushEvent(Event event) -> void {
                m_events.push(event);
            }

            auto popEvent() -> Event {
                auto event = m_events.front();
                m_events.pop();
                return event;
            }

            auto hasEvents() -> bool {
                return !m_events.empty();
            }

            auto pollEvents(RenderWindow& window) -> void {
                window.pollEvents();
            }

        private:
            EventManager() = default;
            ~EventManager() = default;

            std::queue<Event> m_events{};
    };

} // namespace rosa