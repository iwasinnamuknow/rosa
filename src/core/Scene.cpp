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

#include <core/Scene.hpp>
#include <core/components/SpriteComponent.hpp>
#include <core/components/TransformComponent.hpp>

namespace rosa {

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