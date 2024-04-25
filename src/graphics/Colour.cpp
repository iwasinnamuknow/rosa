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

#include <graphics/Colour.hpp>
#include <yaml-cpp/yaml.h>

namespace rosa {

    auto Colour::operator==(const Colour& other) const -> bool {

        const float relative_difference_factor = 0.0001f;// 0.01%

        const auto mag_r   = std::max(std::abs(r), std::abs(other.r));
        auto       r_match = std::abs(r - other.r) <= relative_difference_factor * mag_r;
        const auto mag_g   = std::max(std::abs(g), std::abs(other.g));
        auto       g_match = std::abs(g - other.g) <= relative_difference_factor * mag_g;
        const auto mag_b   = std::max(std::abs(b), std::abs(other.b));
        auto       b_match = std::abs(b - other.b) <= relative_difference_factor * mag_b;
        const auto mag_a   = std::max(std::abs(a), std::abs(other.a));
        auto       a_match = std::abs(a - other.a) <= relative_difference_factor * mag_a;

        return (
                r_match && g_match && b_match && a_match);
    }

    auto operator<<(YAML::Emitter& out, const rosa::Colour& colour) -> YAML::Emitter& {
        out << YAML::Flow;
        out << YAML::BeginSeq << colour.r << colour.g << colour.b << colour.a << YAML::EndSeq;
        return out;
    }

}// namespace rosa
