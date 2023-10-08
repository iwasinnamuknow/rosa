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

#ifdef ROSA_PROFILE

#include <cstdint>
#include <string>
#include <memory>
#include <unordered_map>
#include <chrono>
#include <spdlog/spdlog.h>
#include <iostream>
#include <format>

namespace rosa::debug {

    struct ProfilerSession {
        std::string m_name;

        ProfilerSession(std::string name) : m_name(std::move(name)) {}
    };

    struct ProfilerResult {
        int calls{1};
        int64_t min_time{INT_MAX};
        int64_t max_time{0};
    };

    class Profiler {
        private:
            Profiler() = default;
            std::unique_ptr<ProfilerSession> m_current_session;
            std::unordered_map<std::string, ProfilerResult> m_results;
            
            friend class ProfileScope;

            ~Profiler() {
                endSession();

                for (const auto& [key, result] : m_results) {
                    std::cout << std::format("{},{},{},{}", key, result.calls, result.min_time, result.max_time) << std::endl;
                }
            }

            auto record_result(const std::string& name, int64_t usec) -> void {

                if (!m_current_session) {
                    return;
                }

                if (m_results.contains(name)) {
                    auto& r = m_results.at(name);
                    r.calls++;
                    if (usec < r.min_time) {
                        r.min_time = usec;
                    }
                    if (usec > r.max_time) {
                        r.max_time = usec;
                    }

                } else {
                    m_results.emplace(std::make_pair<std::string, ProfilerResult>(std::string(name), {}));
                }
            }

        public:

            static auto instance() -> Profiler& {
                static Profiler instance;
                return instance;
            }

            auto beginSession(std::string name) -> void {
                
                if (m_current_session) {
                    return;
                }

                m_current_session = std::make_unique<ProfilerSession>(name);
            }

            auto endSession() -> void {
                if (m_current_session) {
                    m_current_session.reset();
                }
            }
    };

    class ProfileScope {
        public:
            ProfileScope(char* name) : m_name(name) {
                m_start_time = std::chrono::high_resolution_clock::now();
            }

            ~ProfileScope() {
                if (!m_stopped) {
                    auto end_time = std::chrono::high_resolution_clock::now();

                    int64_t start = std::chrono::time_point_cast<std::chrono::microseconds>(m_start_time).time_since_epoch().count();
                    int64_t end = std::chrono::time_point_cast<std::chrono::microseconds>(end_time).time_since_epoch().count();

                    Profiler::instance().record_result( m_name, end - start);

                    m_stopped = true;
                }
            }

        private:
            char* m_name;
            std::chrono::time_point<std::chrono::high_resolution_clock> m_start_time;
            bool m_stopped{false};
    };

} // namespace rosa::debug

#define ROSA_PROFILE_SESSION_BEGIN(name)    ::rosa::debug::Profiler::instance().beginSession(name)
#define ROSA_PROFILE_SESSION_END()          ::rosa::debug::Profiler::instance().endSession()
#define ROSA_PROFILE_SCOPE(name)            ::rosa::debug::ProfileScope profile_scope##__LINE__(name)
#else
#define ROSA_PROFILE_SESSION_BEGIN(name)
#define ROSA_PROFILE_SESSION_END()
#define ROSA_PROFILE_SCOPE(name)
#endif // ROSA_PROFILE