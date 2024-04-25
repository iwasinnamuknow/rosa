/*
 * This file is part of rosa.
 *
 *  glad is free software: you can redistribute it and/or modify it under the terms of the
 *  GNU General Public License as published by the Free Software Foundation, either version
 *  3 of the License, or (at your option) any later version.
 *
 *  glad is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without
 *  even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along with rosa. If not,
 *  see <https://www.gnu.org/licenses/>.
 */

#pragma once

#include <cstdint>

namespace rosa {

    /**
     * \brief Represents colour in RGBA format
     *
     * All values range from 0.0 to 1.0
     */
    struct Colour {

        /**
         * \brief Initialise a default colour
         *
         * Defaults are [1, 1, 1, 1]
         */
        constexpr Colour() noexcept = default;

        /**
         * \brief Initialise a colour with three channels 0.F-1.F
         */
        constexpr Colour(float red, float green, float blue) noexcept
            : r(red), g(green), b(blue) {}

        /**
         * \brief Initialise a colour with four channels 0.F-1.F
         */
        constexpr Colour(float red, float green, float blue, float alpha) noexcept
            : r(red), g(green), b(blue), a(alpha) {}

        /**
         * \brief Subtract two colours
         *
         * The result will be clamped at 0.F-1.F
         */
        constexpr auto operator-(const Colour& other) const -> Colour {
            return {
                    std::clamp(r - other.r, 0.F, 1.F),
                    std::clamp(g - other.g, 0.F, 1.F),
                    std::clamp(b - other.b, 0.F, 1.F),
                    std::clamp(a - other.a, 0.F, 1.F),
            };
        }

        /**
         * \brief Add two colours
         *
         * The result will be clamped at 0.F-1.F
         */
        constexpr auto operator+(const Colour& other) const -> Colour {
            return {
                    std::clamp(r + other.r, 0.F, 1.F),
                    std::clamp(g + other.g, 0.F, 1.F),
                    std::clamp(b + other.b, 0.F, 1.F),
                    std::clamp(a + other.a, 0.F, 1.F),
            };
        }

        /**
         * \brief Compare two colours
         *
         * Compare colour channels with a small epsilon
         */
        auto operator==(const Colour& other) const -> bool;

        /**
         * \brief Check if a colour has zeros for all channels
         */
        auto isZero() const noexcept -> bool {
            return (
                    (r == g) && (r == b) && (r == a) && (a == 0.F));
        }

        float r{1}; /**< red value   */
        float g{1}; /**< green value */
        float b{1}; /**< blue value  */
        float a{1}; /**< alpha value */
    };

    auto operator<<(YAML::Emitter& out, const rosa::Colour& colour) -> YAML::Emitter&;

}// namespace rosa

namespace YAML {
    template<>
    struct convert<rosa::Colour> {
        static auto decode(const Node& node, rosa::Colour& rhs) -> bool {
            if (!node.IsSequence() || node.size() != 4) {
                return false;
            }

            rhs.r = node[0].as<float>();
            rhs.g = node[1].as<float>();
            rhs.b = node[2].as<float>();
            rhs.a = node[3].as<float>();
            return true;
        }

        static auto encode(const rosa::Colour& rhs) -> Node {
            Node node;
            node.push_back(rhs.r);
            node.push_back(rhs.g);
            node.push_back(rhs.b);
            node.push_back(rhs.a);
            return node;
        }
    };
}// namespace YAML