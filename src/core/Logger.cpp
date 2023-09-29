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

#include <core/Logger.hpp>

namespace rosa {

    Logger::Logger(std::string path) : m_path(std::move(path)) {        
        m_stream.open(generate_filename(), std::ios::app);

        if (!m_stream) {
            throw Exception(std::format("Couldn't open logfile: {}.", std::strerror(errno)));
        }
    }

    Logger::~Logger() {
        m_stream.close();
    }

    auto Logger::message(const std::string& msg, log_severity severity) -> void {
        m_stream << generate_timestamp() << std::format("{}: ", magic_enum::enum_name(severity)) << msg << "\n";

        // Flush if severity is error or above
        if (static_cast<int>(severity) >= 3) {
            m_stream.flush();
        } 
    }

    auto Logger::generate_filename() -> std::filesystem::path {
        auto time = std::chrono::floor<std::chrono::seconds>(std::chrono::system_clock::now());
        auto today = std::chrono::floor<std::chrono::days>(time);

        std::filesystem::path full_path = m_path;
        full_path /= std::format("rosa-{0:%Y%m%d}-{1:%H%M%S}.log",
            std::chrono::year_month_day{today},
            std::chrono::hh_mm_ss{time - today});

        return full_path;
    }

    auto Logger::generate_timestamp() -> std::string {
        auto time = std::chrono::floor<std::chrono::seconds>(std::chrono::system_clock::now());
        auto today = std::chrono::floor<std::chrono::days>(time);
        auto out = std::format("[{0:%Y-%m-%d}_{1:%H:%M:%S}] ",
                            std::chrono::year_month_day{today},
                            std::chrono::hh_mm_ss{time - today});
        return out;
    }

} // namespace rosa