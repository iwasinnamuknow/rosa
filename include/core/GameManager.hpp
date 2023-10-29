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

    /**
     * The main entrypoint to the engine. Handles scene operations and the main loop.
     *  This is available as a singleton because I'm bad and have dependency issues.
     */
    class GameManager {
        public:
            GameManager(GameManager const &) = delete;
            auto operator=(GameManager const &) -> GameManager & = delete;
            GameManager(GameManager const &&) = delete;
            auto operator=(GameManager const &&) -> GameManager & = delete;

            /**
             * @brief Main game loop.
             *
             *  Responsible for polling events and calling into the current scenes'
             *  update and render handlers.
             *
             *  Also handles ImGUI setup/rendering.
             * 
             */
            auto run() -> void;

            /**
             * @brief Change the currently active scene
             * 
             * @param key the name of the scene to activate
             * @return true if the scene was found
             * @return false if the scene was not found
             */
            auto changeScene(const std::string& key) -> bool;

            /**
             * @brief Add a scene to the pool
             * 
             * @param key the name of the new scene
             * @param scene a unique pointer to the new scene
             * @return true the scene was inserted
             * @return false a scene by the same name already existed
             */
            auto addScene(const std::string& key, std::unique_ptr<Scene> scene) -> bool;

            /**
             * @brief Get the current scene
             * 
             * @return Scene& reference to the scene
             */
            auto getCurrentScene() -> Scene& {
                return *m_current_scene;
            }

            /**
             * @brief Get the render window
             * 
             * @return RenderWindow& reference to the render window
             */
            auto getRenderWindow() -> RenderWindow& {
                return m_render_window;
            }

            /**
             * @brief Singleton accessor
             * 
             * @return GameManager& reference to ourself
             */
            static auto instance() -> GameManager&;

        private:
            explicit GameManager();
            ~GameManager() = default;

            std::chrono::time_point<std::chrono::system_clock> m_time; /**< used to calculate the frame delta time */

            std::unordered_map<std::string, std::unique_ptr<Scene>> m_scenes{}; /**< collection of all the registered scenes */
            Scene* m_current_scene{nullptr};
            RenderWindow m_render_window{};

            //ImGuiIO& io;
    };

} // namespace rosa