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

#include <core/lua_script/CountdownTimer.hpp>
#include <fmt/format.h>

template <class Rep, std::intmax_t num, std::intmax_t denom>
static auto chronoBurst(std::chrono::duration<Rep, std::ratio<num, denom>> d) -> std::string {
    const auto hrs = duration_cast<std::chrono::hours>(d);
    const auto mins = duration_cast<std::chrono::minutes>(d - hrs);
    const auto secs = duration_cast<std::chrono::seconds>(d - hrs - mins);
    const auto ms = duration_cast<std::chrono::milliseconds>(d - hrs - mins - secs);

    return fmt::format("{:02d}:{:02d}:{:02d}.{:03d}", hrs.count(), mins.count(), secs.count(), ms.count());
}

namespace rosa {

    namespace lua_script {

        CountdownTimer::CountdownTimer(int seconds) 
            : m_start_time(std::chrono::steady_clock::now() + std::chrono::seconds(seconds)) {

        }

        auto CountdownTimer::getSeconds() -> int {
            m_current_time = std::chrono::steady_clock::now();
            auto duration = m_start_time - m_current_time;

            return std::chrono::duration_cast<std::chrono::seconds>(duration).count();
        }

        auto CountdownTimer::getFormatted() -> std::string {
            m_current_time = std::chrono::steady_clock::now();
            auto duration = m_start_time - m_current_time;

            return chronoBurst(duration);
        }

        auto CountdownTimer::getFinished() -> bool {
            m_current_time = std::chrono::steady_clock::now();
            auto duration = m_start_time - m_current_time;
            
            return std::chrono::duration_cast<std::chrono::nanoseconds>(duration).count() <= 0;
        }

    }

} // namespace rosa