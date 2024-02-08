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

namespace {
    template<class Rep, std::intmax_t Num, std::intmax_t Denom>
    auto chrono_burst(std::chrono::duration<Rep, std::ratio<Num, Denom>> duration) -> std::string {
        const auto hours   = duration_cast<std::chrono::hours>(duration);
        const auto minutes = duration_cast<std::chrono::minutes>(duration - hours);
        const auto seconds = duration_cast<std::chrono::seconds>(duration - hours - minutes);
        const auto millis  = duration_cast<std::chrono::milliseconds>(duration - hours - minutes - seconds);

        return fmt::format("{:02d}:{:02d}:{:02d}.{:03d}", hours.count(), minutes.count(), seconds.count(), millis.count());
    }
}// namespace

namespace rosa::lua_script {

    CountdownTimer::CountdownTimer(int seconds)
        : m_start_time(std::chrono::steady_clock::now() + std::chrono::seconds(seconds)) {
    }

    auto CountdownTimer::getSeconds() -> int {
        m_current_time = std::chrono::steady_clock::now();
        auto duration  = m_start_time - m_current_time;

        return static_cast<int>(std::chrono::duration_cast<std::chrono::seconds>(duration).count());
    }

    auto CountdownTimer::getFormatted() -> std::string {
        m_current_time = std::chrono::steady_clock::now();
        auto duration  = m_start_time - m_current_time;

        return chrono_burst(duration);
    }

    auto CountdownTimer::getFinished() -> bool {
        m_current_time = std::chrono::steady_clock::now();
        auto duration  = m_start_time - m_current_time;

        return std::chrono::duration_cast<std::chrono::nanoseconds>(duration).count() <= 0;
    }

}// namespace rosa::lua_script