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

#include <core/Uuid.hpp>
#include <random>

namespace rosa {

    Uuid::Uuid(const std::string& str_uuid) {

        if (str_uuid.length() != 32) {
            throw Exception(fmt::format("Invalid UUID: {}", str_uuid));
        }
        
        auto first_half =  str_uuid.substr(0, 16);
        auto second_half = str_uuid.substr(16, 32);

        try {
            m_top = std::stoull(first_half, nullptr, 16);
            m_bottom = std::stoull(second_half, nullptr, 16);

        } catch (std::exception e) {
            throw Exception(fmt::format("Invalid UUID: {}", str_uuid));
        }
    }

    auto Uuid::operator==(const Uuid& other) const -> bool {
        return (m_top == other.m_top && m_bottom == other.m_bottom);
    }

    Uuid::operator std::string() const {
        return fmt::format("{:16x}{:16x}", m_top, m_bottom);
    }

    auto Uuid::generate() -> Uuid {
        std::random_device rd;
        auto seed_data = std::array<int, std::mt19937_64::state_size> {};
        std::generate(std::begin(seed_data), std::end(seed_data), std::ref(rd));
        std::seed_seq seq(std::begin(seed_data), std::end(seed_data));
        std::mt19937_64 generator(seq);
        return Uuid(fmt::format("{:16x}{:16x}", generator(), generator()));
    }

    auto Uuid::getLower() -> u_int64_t {
        return m_bottom;
    }

    auto Uuid::getUpper() -> u_int64_t {
        return m_top;
    }

    auto Uuid::toString() -> std::string {
        return static_cast<std::string>(*this);
    }

} // namespace rosa
