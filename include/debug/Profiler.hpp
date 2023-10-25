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

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <graphics/gl.hpp>
#include <GLFW/glfw3.h>

#include <cstdint>
#include <string>
#include <memory>
#include <unordered_map>
#include <chrono>
#include <spdlog/spdlog.h>
#include <iostream>
#include <fstream>
#include <format>

namespace rosa::debug {

    enum ProfileUiItemColumnId {
        profile_item_name,
        profile_item_last,
    };

    struct ProfileUiItem {
        int id;
        std::string name;
        int64_t last;

    };

    struct ProfilerResult {
        int calls{0};
        int64_t min_time{INT_MAX};
        int64_t max_time{0};
        int64_t total_time{0};
        int64_t avg_time{0};
        int64_t last_time{0};
    };

    class Profiler {
        private:
            Profiler() = default;
            std::string m_session_name;
            std::unordered_map<std::string, ProfilerResult> m_results;
            std::unordered_map<std::string, ProfilerResult> m_last_result;
            bool m_running{false};
            
            friend class ProfileScope;

            ~Profiler() {
                auto filename = std::format("profile-{}.csv", m_session_name);
                std::ofstream fout(filename);
                endSession();

                fout << "Function,Calls,Last Time,Min Time,Avg Time,Max Time\n";

                for (const auto& [key, result] : m_results) {
                    fout << std::format("{},{},{},{},{},{}", key, result.calls, result.last_time, result.min_time, result.avg_time, result.max_time) << "\n";
                }
            }

            auto record_result(const std::string& name, int64_t usec) -> void {

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

        public:

            static auto instance() -> Profiler& {
                static Profiler instance;
                return instance;
            }

            auto beginSession(std::string name) -> void {
                
                if (m_running) {
                    return;
                }

                m_running = true;
                m_session_name = std::move(name);
            }

            auto endSession() -> void {
                if (m_running) {
                    m_running = false;
                }
            }

            auto getEntries() -> const std::unordered_map<std::string, ProfilerResult>& {
                return m_results;
            }

            auto getLastFrame() -> const std::unordered_map<std::string, ProfilerResult>& {
                return m_last_result;
            }

            auto clearLastFrame() -> void {
                m_last_result.clear();
            }
    };

    class ProfileScope {
        public:
            ProfileScope(const char* name) : m_name(name) {
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
            const char* m_name;
            std::chrono::time_point<std::chrono::high_resolution_clock> m_start_time;
            bool m_stopped{false};
    };

} // namespace rosa::debug

#define TOKENPASTE(x, y) x ## y
#define TOKENPASTE2(x, y) TOKENPASTE(x, y)
#define ROSA_PROFILE_SESSION_BEGIN(name)    ::rosa::debug::Profiler::instance().beginSession(name)
#define ROSA_PROFILE_SESSION_END()          ::rosa::debug::Profiler::instance().endSession()
#define ROSA_PROFILE_SCOPE(name)            ::rosa::debug::ProfileScope TOKENPASTE2(profile_scope, __LINE__)(name)
#else
#define ROSA_PROFILE_SESSION_BEGIN(name)
#define ROSA_PROFILE_SESSION_END()
#define ROSA_PROFILE_SCOPE(name)
#endif // ROSA_PROFILE