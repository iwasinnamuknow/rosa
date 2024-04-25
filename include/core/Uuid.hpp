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

#pragma once

#include <core/Exception.hpp>

#include <algorithm>
#include <cstdint>
#include <cstring>
#include <ostream>
#include <random>
#include <string>

// Forward declare UUID for hash
namespace rosa {
    class Uuid;
}

// Forward declare hash for UUID
template<>
struct std::hash<rosa::Uuid>;

namespace rosa {

    static std::unique_ptr<std::mt19937_64>             s_generator{nullptr};
    static std::uniform_int_distribution<std::uint64_t> s_dist{};

    /**
     * \brief UUIDv4 for entities and assets
     */
    class Uuid final {
    public:
        /**
         * \brief Create a zero-initialised UUID object
         */
        constexpr Uuid() noexcept = default;

        /**
         * \brief Create a UUID object from a string of characters
         * \param str_uuid string representation
         */
        constexpr Uuid(const std::string& str_uuid) {

            std::string tmpstr = str_uuid;

            if (tmpstr.length() == 36) {
                // Remove dashes
                tmpstr.erase(
                        std::remove_if(
                                tmpstr.begin(),
                                tmpstr.end(),
                                [](char i) {
                                    if (i == '-') {
                                        return true;
                                    }

                                    return false;
                                }),
                        tmpstr.end());
            }

            if (tmpstr.length() != 32) {
                throw Exception("UUID too short: " + str_uuid);
            }

            // Copy each byte into the array
            for (std::uint64_t index{0}; index < 16; index++) {
                auto substr      = tmpstr.substr(index * 2, 2);
                m_data.at(index) = hexByteToInt(substr);// can't constexpr stoi so had to reimplement
            }
        }

        /**
         * \brief Create a UUID object from an array of bytes
         * \param bytes An array of 16 raw byte values
         */
        constexpr Uuid(const std::array<std::uint8_t, 16> bytes)
            : m_data(bytes) {}

        // Copy operators
        constexpr Uuid(const Uuid& other)                    = default;
        constexpr auto operator=(const Uuid& other) -> Uuid& = default;

        // Destructor
        constexpr ~Uuid() noexcept = default;

        /**
         * \brief Generate a UUID from random bytes
         */
        static auto generate() -> Uuid {
            if (s_generator == nullptr) {
                s_generator = std::make_unique<std::mt19937_64>(std::random_device()());
                s_dist      = std::uniform_int_distribution(std::numeric_limits<std::uint64_t>::min(), std::numeric_limits<std::uint64_t>::max());
            }

            // Grab two 64-bit values
            auto top    = s_dist(*s_generator);
            auto bottom = s_dist(*s_generator);

            // Split into 16 bytes
            std::array<std::uint8_t, 16> bytes{0};

            std::memcpy(bytes.data(), &top, sizeof(top));
            std::memcpy(bytes.data() + (sizeof(std::uint8_t) * 8), &bottom, sizeof(bottom));

            // Set version and variant
            bytes[6] = ((bytes[6] & 0x0fu) | 0x40u);// Version 4
            bytes[8] = ((bytes[8] & 0x3fu) | 0x80u);// Variant is 10

            return {bytes};
        }

        // Stream output operator
        friend auto operator<<(std::ostream& out, const rosa::Uuid& other) -> std::ostream& {
            return out << static_cast<std::string>(other);
        }

        // Equality operator
        constexpr auto operator==(const Uuid& other) const noexcept -> bool {
            const auto& o_data = other.getData();

            // Compare byte-for-byte
            for (std::uint64_t index{0}; index < 16; index++) {
                if (m_data[index] != o_data[index]) {
                    return false;
                }
            }

            return true;
        }

        // std::string operator
        operator std::string() const {
            // Formats to "0065e7d7-418c-4da4-b4d6-b54b6cf7466a"
            std::array<char, 256> buffer        = {0};
            auto                  chars_written = std::snprintf(buffer.data(), 255,
                                                                "%02x%02x%02x%02x-%02x%02x-%02x%02x-%02x%02x-%02x%02x%02x%02x%02x%02x",
                                                                m_data[0], m_data[1], m_data[2], m_data[3],
                                                                m_data[4], m_data[5],
                                                                m_data[6], m_data[7],
                                                                m_data[8], m_data[9],
                                                                m_data[10], m_data[11], m_data[12], m_data[13], m_data[14], m_data[15]);

            if (chars_written < 0 || chars_written == 256) {
                throw Exception("Failed to stringify UUID");
            }

            return {buffer.data()};
        }

        /**
         * \brief Get as raw bytes
         * \return A 16-byte array
         */
        constexpr auto getData() const noexcept -> const std::array<std::uint8_t, 16>& {
            return m_data;
        }

        /**
         * \brief Convert UUID to a string
         * \return std::string representation
         */
        auto toString() const -> std::string {
            return static_cast<std::string>(*this);
        }

        constexpr auto empty() const -> bool {
            for (std::uint64_t index{0}; index < 16; index++) {
                if (m_data[index] != 0) {
                    return false;
                }
            }

            return true;
        }

    private:
        // Array of bytes to hold the UUID value
        std::array<std::uint8_t, 16> m_data{0};

        // Allow access from hashing function
        friend std::hash<rosa::Uuid>;

        // consexpr impl of hex byte to uint8
        constexpr auto hexByteToInt(const std::string_view view) -> std::uint8_t {

            std::uint8_t result{0};
            int          shift{4};

            for (const auto character: view) {
                if (character >= 48 && character <= 57) {// regular digit
                    result |= ((character - 48) << shift);

                } else if (character >= 65 && character <= 70) {// uppercase A-F
                    result |= ((character - 65 + 10) << shift);

                } else if (character >= 97 && character <= 102) {// lowercase a-f
                    result |= ((character - 97 + 10) << shift);
                }

                shift -= 4;
            }

            return result;
        }
    };

}// namespace rosa

/**
 * \brief Hashing function for UUIDs
 */
template<>
struct std::hash<rosa::Uuid> {
    std::size_t operator()(const rosa::Uuid& k) const {
        using std::hash;
        using std::size_t;
        using std::string;

        std::size_t res{0};

        for (const auto& element: k.getData()) {
            res = res * 31 + hash<std::uint8_t>()(element);
        }

        return res;
    }
};

/**
 * \brief Serialisation functions
 */
namespace YAML {
    template<>
    struct convert<rosa::Uuid> {
        static auto decode(const Node& node, rosa::Uuid& rhs) -> bool {
            rhs = node.as<std::string>();
            return true;
        }

        static auto encode(const rosa::Uuid& rhs) -> Node {
            Node node;
            node = rhs.toString();
            return node;
        }
    };
}// namespace YAML