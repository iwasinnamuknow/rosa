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

#include <core/Exception.hpp>

#include <string>
#include <fmt/format.h>
#include <ostream>

namespace rosa {
    class Uuid;
}

template <>
struct std::hash<rosa::Uuid>;

namespace rosa {

    class Uuid {
        public:
            Uuid() = default;
            Uuid(const std::string& str_uuid);
            Uuid(const Uuid& other) = default;

            friend auto operator<<(std::ostream& out, const rosa::Uuid& other) -> std::ostream& {
                return out << static_cast<std::string>(other);
            }
            auto operator==(const Uuid& other) const -> bool;
            explicit operator std::string() const;

            static auto generate() -> Uuid;

            auto getLower() -> u_int64_t;
            auto getUpper() -> u_int64_t;

            auto toString() -> std::string;
            
        private:
            u_int64_t m_top{0};
            u_int64_t m_bottom{0};

            friend std::hash<rosa::Uuid>;
    };

} // namespace rosa

template <>
struct std::hash<rosa::Uuid>
{
    std::size_t operator()(const rosa::Uuid& k) const
    {
        using std::size_t;
        using std::hash;
        using std::string;

        // Compute individual hash values for first, second and third
        // http://stackoverflow.com/a/1646913/126995
        std::size_t res = 17;
        res = res * 31 + hash<uint64_t>()( k.m_top );
        res = res * 31 + hash<uint64_t>()( k.m_bottom );
        return res;
    }
};