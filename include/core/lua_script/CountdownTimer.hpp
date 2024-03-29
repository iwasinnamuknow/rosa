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

#include <chrono>
#include <string>

namespace rosa {

    namespace lua_script {

        class CountdownTimer {
            public:
                CountdownTimer(int seconds);
                ~CountdownTimer() = default;

                auto getSeconds() -> int;
                auto getFormatted() -> std::string;
                auto getFinished() -> bool;

            private:
                std::chrono::time_point<std::chrono::steady_clock> m_start_time;
                std::chrono::time_point<std::chrono::steady_clock> m_current_time;
        };

    } // namespace lua_script

} // namespace rosa