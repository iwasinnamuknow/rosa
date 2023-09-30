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
#include <cerrno>
#include <cstring>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <format>
#include <string>

#include <core/Exception.hpp>
#include <magic_enum.hpp>

namespace rosa {

    enum log_severity {
        debug       = 0,
        info        = 1,
        warning     = 2,
        error       = 3,
        critical    = 4
    };

    class Logger {
        public:
            explicit Logger(std::string path);
            Logger(Logger const&) = delete;
            auto operator=(Logger const&) -> Logger& = delete;
            Logger(Logger const&&) = delete;
            auto operator=(Logger const&&) -> Logger& = delete;
            ~Logger();

            auto message(const std::string& msg, log_severity severity = log_severity::info) -> void;

        private:
            std::string m_path;
            std::fstream m_stream;

            auto generate_filename() -> std::filesystem::path;
            auto generate_timestamp() -> std::string;
    };

} // namespace rosa