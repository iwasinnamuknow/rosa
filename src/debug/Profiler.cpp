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

#include <debug/Profiler.hpp>

namespace rosa::debug {

    Profiler::~Profiler() {
        // auto filename = fmt::format("profile-{}.csv", m_session_name);
        // std::ofstream fout(filename);
        // endSession();

        // fout << "Function,Calls,Last Time,Min Time,Avg Time,Max Time\n";

        // for (const auto& [key, result] : m_results) {
        //     fout << fmt::format("{},{},{},{},{},{}", key, result.calls, result.last_time, result.min_time, result.avg_time, result.max_time) << "\n";
        // }
    }

    auto Profiler::beginSession(std::string name) -> void {
        if (m_running) {
            return;
        }

        m_running = true;
        m_session_name = std::move(name);
    }

    auto Profiler::endSession() -> void {
        if (m_running) {
            m_running = false;
        }
    }

    auto Profiler::record_result(const std::string& name, int64_t usec) -> void {
        if (!m_running) {
            return;
        }

        if (m_last_result.contains(name)) {
            auto& result = m_last_result.at(name);
            result.last_time = usec;
        } else {
            ProfilerResult result{};
            result.last_time = usec;
            m_last_result.insert_or_assign(std::string(name), result);
        }

        if (m_results.contains(name)) {
            auto& result = m_results.at(name);
            result.calls++;
            if (usec < result.min_time) {
                result.min_time = usec;
            }
            if (usec > result.max_time) {
                result.max_time = usec;
            }
            result.total_time += usec;
            result.avg_time = result.total_time / result.calls;
            result.last_time = usec;
        } else {
            ProfilerResult result{};
            result.calls++;
            if (usec < result.min_time) {
                result.min_time = usec;
            }
            if (usec > result.max_time) {
                result.max_time = usec;
            }
            result.total_time += usec;
            result.avg_time = result.total_time / result.calls;
            result.last_time = usec;
            m_results.insert_or_assign(std::string(name), result);
        }
    }

} // namespace rosa::debug