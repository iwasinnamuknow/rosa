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

#include "core/ResourceManager.hpp"
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Vector2.hpp>
#include <core/Scene.hpp>
#include <core/components/SpriteComponent.hpp>
#include <core/components/TransformComponent.hpp>
#include <entt/entity/fwd.hpp>

namespace rosa {

    Scene::Scene(ResourceManager& resource_manager, sf::RenderWindow& render_window) : m_resource_manager(resource_manager), m_render_window(render_window) {
        auto entity = getRegistry().create();
        getRegistry().emplace<SpriteComponent>(entity);
        auto& texture = m_resource_manager.getTexture("assets/rosa.png");
        getRegistry().get<SpriteComponent>(entity).sprite.setTexture(texture);

        const sf::Vector2f position = sf::Vector2f(
            (static_cast<float>(m_render_window.getSize().x) / 2.F) - (static_cast<float>(texture.getSize().x) / 2.F),
            (static_cast<float>(m_render_window.getSize().y) / 2.F) - (static_cast<float>(texture.getSize().y) / 2.F)
        );
        getRegistry().emplace<TransformComponent>(entity, position, sf::Vector2f(0,0), sf::Vector2f(1,1), 0);
    }

    auto Scene::update(float /*delta_time*/) -> void {
        // This function only cares about entities with SpriteComponent and TransformComponent
        auto view = m_registry.view<SpriteComponent, TransformComponent>();

        for (const auto& entity : view)
        {
            const TransformComponent& transform = m_registry.get<TransformComponent>(entity);
            SpriteComponent& sprite_comp = m_registry.get<SpriteComponent>(entity);

            sprite_comp.sprite.setPosition(transform.position);
            sprite_comp.sprite.setScale(transform.scale);
            sprite_comp.sprite.setRotation(transform.rotation);
        }
    }

    auto Scene::render(sf::RenderWindow& window) -> void {
        // For every entity with a SpriteComponent, draw it.
        m_registry.view<SpriteComponent>().each([&](const auto& sprite_comp)
        {
            window.draw(sprite_comp.sprite);
        });
    }

} // namespace rosa