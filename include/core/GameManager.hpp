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

#include <unordered_map>
#include <core/Scene.hpp>
#include <SFML/Graphics.hpp>
#include <core/ResourceManager.hpp>

namespace rosa {

    class GameManager {
        public:
            explicit GameManager();
            GameManager(GameManager const &) = delete;
            auto operator=(GameManager const &) -> GameManager & = delete;
            GameManager(GameManager const &&) = delete;
            auto operator=(GameManager const &&) -> GameManager & = delete;
            ~GameManager();

            auto run() -> void;
            auto changeScene(const std::string& key) -> bool;

        private:
            std::unordered_map<std::string, Scene> m_scenes{};
            Scene* m_current_scene{nullptr};
            sf::RenderWindow m_render_window{};
            ResourceManager m_resource_manager{};
    };

} // namespace rosa