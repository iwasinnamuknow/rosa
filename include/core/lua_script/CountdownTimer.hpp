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
 *  You should have received a copy of the GNU General Public License along with bbai. If not,
 *  see <https://www.gnu.org/licenses/>.
 */

#pragma once

#include <chrono>
#include <sol/sol.hpp>

template <class Rep, std::intmax_t num, std::intmax_t denom>
auto chronoBurst(std::chrono::duration<Rep, std::ratio<num, denom>> d) -> sol::table {
    const auto hrs = duration_cast<std::chrono::hours>(d);
    const auto mins = duration_cast<std::chrono::minutes>(d - hrs);
    const auto secs = duration_cast<std::chrono::seconds>(d - hrs - mins);
    const auto ms = duration_cast<std::chrono::milliseconds>(d - hrs - mins - secs);

    sol::table table;
    table["hours"] = hrs;
    table["minutes"] = mins;
    table["seconds"] = secs;
    table["milliseconds"] = ms;

    return table;
}

namespace rosa {

    namespace lua_script {

        class CountdownTimer {
            public:
                CountdownTimer(int seconds);
                ~CountdownTimer() = default;

                auto getSeconds() -> int;
                auto getFormatted() -> sol::table;

            private:
                std::chrono::time_point<std::chrono::steady_clock> m_start_time;
                std::chrono::time_point<std::chrono::steady_clock> m_current_time;
                bool m_finished{false};
        };

    } // namespace lua_script

} // namespace rosa