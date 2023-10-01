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

#include <SFML/Graphics/RenderWindow.hpp>
#include <boost/di.hpp>
#include <entt/entt.hpp>
#include <core/ResourceManager.hpp>

namespace rosa {

    class Scene {
        public:
            explicit Scene(ResourceManager& resource_manager, sf::RenderWindow& render_window);
            auto update(float delta_time) -> void;
            auto render(sf::RenderWindow& window) -> void;

        protected:
            auto getRegistry() -> entt::registry& {
                return m_registry;
            }
        private:
            entt::registry m_registry{};
            ResourceManager& m_resource_manager;
            sf::RenderWindow& m_render_window;
    };

} // namespace rosa