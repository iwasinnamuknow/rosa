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

#include <core/Resource.hpp>

#include <array>

static constexpr int bf_max_string_length{256};
static constexpr int bf_width_data_offset{20};
static constexpr int bf_map_data_offset{276};

namespace rosa {

    class BitmapFont : public Resource {
    public:
        BitmapFont(const std::string &name, const Uuid &uuid, const std::string &pack) : Resource(name, uuid, pack) {}
        auto loadFromPhysFS() -> bool override;

        auto print(const std::string& text, int pos_x, int pos_y, Colour colour = Colour{1.F, 1.F, 1.F}) -> std::vector<Quad>;
        auto getWidth(const std::string& text) -> int;

    protected:
        auto setCursor(int x, int y) -> void;
        auto bind() -> void;
        auto setBlending() -> void;
        auto select() -> void;
        auto reverseYAxis(bool state) -> void;
        auto generateCharQuads(const std::string& text, Colour colour = Colour{1.F, 1.F, 1.F}) -> std::vector<Quad>;

        unsigned int m_texture_id{};
        int m_cell_x{};
        int m_cell_y{};
        int m_y_offset{};
        int m_row_pitch{};
        int m_cursor_x{0};
        int m_cursor_y{0};
        char m_base_char{};
        std::array<char, 256> m_widths{};
        float m_row_factor{};
        float m_col_factor{};
        int m_render_style{};
        bool m_invert_y_axis{false};
        int m_image_x{0};
        int m_image_y{0};
    };

} // namespace rosa