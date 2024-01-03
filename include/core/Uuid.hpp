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
#include <cstdint>
#include <span>

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

            auto getData() const -> const std::array<std::uint8_t, 16>&;

            auto toString() -> std::string;
            
        private:
            std::array<std::uint8_t, 16> m_data{0};

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

        std::size_t res{0};

        for (const auto& element : k.getData()) {
            res = res * 31 + hash<std::uint8_t>()(element);
        }

        return res;
    }
};