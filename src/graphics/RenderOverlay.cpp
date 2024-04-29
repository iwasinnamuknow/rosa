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

#include <algorithm>
#include <graphics/RenderOverlay.hpp>

namespace rosa {

    auto RenderOverlay::setVisible(bool visible) -> void {
        m_visible = visible;
    }

    auto RenderOverlay::getVisible() -> bool {
        return m_visible;
    }

    auto RenderOverlay::toggleVisible() -> void {
        m_visible = !m_visible;
    }

    auto RenderOverlay::update(float delta_time) -> void {
        if (m_renderer_stats.size() >= 120) {
            m_renderer_stats.pop_back();
        }

        if (m_frame_times.size() >= 120) {
            m_frame_times.pop_back();
        }

        RendererStats stats = Renderer::getInstance().getStats();
        m_renderer_stats.push_front(stats);
        m_frame_times.push_front(delta_time);

        std::fill(m_draw_calls.begin(), m_draw_calls.end(), 0);
        std::fill(m_vertices.begin(), m_vertices.end(), 0);
        std::fill(m_textures.begin(), m_textures.end(), 0);
        std::fill(m_shaders.begin(), m_shaders.end(), 0);
        std::fill(m_fps.begin(), m_fps.end(), 0);

        if (m_visible) {
            ImGui::SetNextWindowPos(ImVec2(0, 0));
            ImGui::SetNextWindowSize(ImVec2(302, 500));
            ImGui::Begin("Renderer Stats", nullptr,
                         ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoBackground);

            {
                std::size_t index{0};
                for (const auto& time: m_frame_times) {
                    auto fps     = 1.F / time;
                    m_fps[index] = fps;
                    index++;
                }
            }

            {
                std::size_t index{0};
                for (const auto& entry: m_renderer_stats) {

                    m_draw_calls[index] = static_cast<float>(entry.draws);
                    m_vertices[index]   = static_cast<float>(entry.vertices);
                    m_textures[index]   = static_cast<float>(entry.textures);
                    m_shaders[index]    = static_cast<float>(entry.shaders);

                    index++;
                }
            }

            ImGui::Text("Framerate %.1f", 1.F / delta_time);
            ImGui::PlotLines("", m_fps.data(), 120, 0, nullptr, 0.F, 1.F, ImVec2(300.F, 50.F));
            ImGui::NewLine();

            ImGui::Text("Draw Calls %d", stats.draws);
            ImGui::PlotLines("", m_draw_calls.data(), 120, 0, nullptr, 0.F, 1.F, ImVec2(300.F, 50.F));
            ImGui::NewLine();

            ImGui::Text("Vertices %d", stats.vertices);
            ImGui::PlotLines("", m_vertices.data(), 120, 0, nullptr, 0.F, 1.F, ImVec2(300.F, 50.F));
            ImGui::NewLine();

            ImGui::Text("Texture Binds %d", stats.textures);
            ImGui::PlotLines("", m_textures.data(), 120, 0, nullptr, 0.F, 1.F, ImVec2(300.F, 50.F));
            ImGui::NewLine();

            ImGui::Text("Shader Binds %d", stats.shaders);
            ImGui::PlotLines("", m_shaders.data(), 120, 0, nullptr, 0.F, 1.F, ImVec2(300.F, 50.F));

            ImGui::End();
        }
    }
}// namespace rosa
