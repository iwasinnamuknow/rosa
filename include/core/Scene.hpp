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
#include <entt/entt.hpp>
#include <core/ResourceManager.hpp>
#include <spdlog/spdlog.h>
#include <unordered_map>
#include <core/Entity.hpp>

namespace rosa {

    class NativeScriptEntity;
    class SceneSerialiser;

    class Scene {
        public:
            explicit Scene(sf::RenderWindow& render_window);
            virtual auto update(float delta_time) -> void;
            virtual auto render() -> void;
            virtual ~Scene() = default;

            auto getRenderWindow() const -> sf::RenderWindow& {
                return m_render_window;
            }

        protected:
            auto getRegistry() -> entt::registry& {
                return m_registry;
            }

            auto createEntity() -> Entity;
            auto removeEntity(Entity& entity) -> bool;
        private:
            entt::registry m_registry{};
            sf::RenderWindow& m_render_window;
            std::unordered_map<entt::entity, Entity> m_entities{};

            auto create_entity(uuids::uuid uuid) -> Entity;

            friend class NativeScriptEntity;
            friend class Entity;
            friend class SceneSerialiser;
    };

} // namespace rosa