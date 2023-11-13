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

#include "graphics/Colour.hpp"
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

    /**
     * The main entrypoint to the engine. Handles scene operations and the main loop.
     *  This is available as a singleton because I'm bad and have dependency issues.
     */
    class GameManager {
        public:

            /**
             * @brief Construct a new Game Manager with the desired window size
             * 
             * @param window_width in pixels
             * @param window_height in pixels
             */
            explicit GameManager(int window_width, int window_height, const std::string& window_title = "rosa window");
            
            ~GameManager() = default;

            GameManager(GameManager const &) = delete;
            auto operator=(GameManager const &) -> GameManager & = delete;
            GameManager(GameManager const &&) = delete;
            auto operator=(GameManager const &&) -> GameManager & = delete;

            /**
             * @brief Main initialisation and game loop.
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
             * @brief Load a previously serialised scene
             * 
             * @param key the name of the new scene
             * @param path yaml file containing scene
             * @return true the scene was unpacked
             * @return false the scene couldn't be unpacked or the name already exists
             */
            auto unpackScene(const std::string& key, const std::string& path) -> bool;

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
             * @brief Set the colour used to clear the screen each frame
             * 
             * @param colour rgb(a) values for the clear colour
             */
            auto setClearColour(Colour colour) -> void {
                m_clear_colour = colour;
            }

        private:

            std::chrono::time_point<std::chrono::system_clock> m_time; /**< used to calculate the frame delta time */

            std::unordered_map<std::string, std::unique_ptr<Scene>> m_scenes{}; /**< collection of all the registered scenes */
            Scene* m_current_scene{nullptr};
            RenderWindow m_render_window{};

            Colour m_clear_colour{0,0,0,1};

            bool m_initialised{false};
    };

} // namespace rosa