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

#include <graphics/Renderer.hpp>
#include <list>

namespace rosa {

    class RenderOverlay {
    public:
        auto setVisible(bool visible) -> void;
        auto getVisible() -> bool;
        auto toggleVisible() -> void;
        auto update(float delta_time) -> void;

    private:
        std::list<RendererStats> m_renderer_stats;
        std::list<float>         m_frame_times;

        std::array<float, 120> m_draw_calls{0};
        std::array<float, 120> m_vertices{0};
        std::array<float, 120> m_textures{0};
        std::array<float, 120> m_shaders{0};
        std::array<float, 120> m_fps{0};

        bool m_visible{false};
    };

}// namespace rosa
