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
#include <algorithm>
#include <sstream>

namespace rosa {

    Uuid::Uuid(const std::string& str_uuid) {

        std::string tmpstr = str_uuid;

        if (str_uuid.length() == 36) {
            // Remove dashes
            std::erase(tmpstr, '-');
        }

        if (tmpstr.length() != 32) {
            throw Exception(fmt::format("Invalid UUID: {}", str_uuid));
        }

        for (unsigned int index{0}; index < 16; index++) {
            auto substr = tmpstr.substr(index * 2, 2);
            m_data.at(index) = std::stoul(substr, nullptr, 16);
        }
    }

    auto Uuid::operator==(const Uuid& other) const -> bool {
        auto& o_data = other.getData();

        for (int index{0}; index < 16; index++) {
            if (m_data[index] != o_data[index]) {
                return false;
            }
        }

        return true;
    }

    Uuid::operator std::string() const {
        // Formats to "0065e7d7-418c-4da4-b4d6-b54b6cf7466a"
        char buffer[256] = {0};
        std::snprintf(buffer, 255,
                      "%02x%02x%02x%02x-%02x%02x-%02x%02x-%02x%02x-%02x%02x%02x%02x%02x%02x",
                      m_data[0], m_data[1], m_data[2], m_data[3],
                      m_data[4], m_data[5],
                      m_data[6], m_data[7],
                      m_data[8], m_data[9],
                      m_data[10], m_data[11], m_data[12], m_data[13], m_data[14], m_data[15]);

        return {buffer};
    }

    auto Uuid::generate() -> Uuid {
        std::random_device rd;
        auto seed_data = std::array<int, std::mt19937_64::state_size> {};
        std::generate(std::begin(seed_data), std::end(seed_data), std::ref(rd));
        std::seed_seq seq(std::begin(seed_data), std::end(seed_data));
        std::mt19937_64 generator(seq);
        std::uint64_t top = generator();
        std::uint64_t bottom = generator();
        auto uuid = Uuid(fmt::format("{:016x}{:016x}", top, bottom));
        uuid.m_data[6] = ((uuid.m_data[6] & 0x0f) | 0x40); // Version 4
        uuid.m_data[8] = ((uuid.m_data[8] & 0x3f) | 0x80); // Variant is 10
        return uuid;
    }

    auto Uuid::getData() const -> const std::array<std::uint8_t, 16>& {
        return m_data;
    }

    auto Uuid::toString() -> std::string {
        return static_cast<std::string>(*this);
    }

} // namespace rosa
