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

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <graphics/gl.hpp>
#include <GLFW/glfw3.h>

#include <unordered_map>
#include <core/Scene.hpp>
#include <core/ResourceManager.hpp>
#include <graphics/RenderWindow.hpp>
#include <spdlog/spdlog.h>
#include <memory>

namespace rosa {
    
    constexpr int window_width{800};
    constexpr int window_height{600};

    class GameManager {
        public:
            GameManager(GameManager const &) = delete;
            auto operator=(GameManager const &) -> GameManager & = delete;
            GameManager(GameManager const &&) = delete;
            auto operator=(GameManager const &&) -> GameManager & = delete;

            auto run() -> void;
            auto changeScene(const std::string& key) -> bool;
            auto addScene(const std::string& key, std::unique_ptr<Scene> scene) -> bool;
            auto getCurrentScene() -> Scene& {
                return *m_current_scene;
            }

            auto getRenderWindow() -> RenderWindow& {
                return m_render_window;
            }

            static auto instance() -> GameManager&;

        private:
            explicit GameManager();
            ~GameManager();

            std::chrono::time_point<std::chrono::system_clock> m_time;

            std::unordered_map<std::string, std::unique_ptr<Scene>> m_scenes{};
            Scene* m_current_scene{nullptr};
            RenderWindow m_render_window{};

            //ImGuiIO& io;
    };

} // namespace rosa