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

#include <SFML/System/Vector2.hpp>
#include <core/ResourceManager.hpp>
#include <core/Scene.hpp>
#include <core/Entity.hpp>
#include <core/components/TransformComponent.hpp>
#include <core/components/NativeScriptComponent.hpp>
#include <entt/entity/fwd.hpp>
#include <functional>

#include "imgui.h"
#include "imgui-SFML.h"

namespace rosa {

    Scene::Scene(sf::RenderWindow& render_window) : m_render_window(render_window) { }

    auto Scene::createEntity() -> Entity {
        Entity entity{m_registry.create(), std::reference_wrapper<entt::registry>(m_registry)};
        entity.addComponent<TransformComponent>();
        m_entities.insert({entity.getId(), entity});
        return entity;
    };
    
    auto Scene::removeEntity(entt::entity entity) -> bool {
        if (m_entities.contains(entity)) {
            m_entities.at(entity).m_for_deletion = true;
            return true;
        }

        return false;
    }

    auto Scene::update(float delta_time) -> void {

        // Run updates for native script components, instantiating where needed
        m_registry.view<NativeScriptComponent>().each([this, delta_time](entt::entity entity, auto& nsc) {

            Entity* actual = &m_entities.at(entity);

            if (!nsc.instance) {
                nsc.instantiate_function(std::reference_wrapper<Scene>(*this), std::reference_wrapper<Entity>(*actual));
                nsc.on_create_function(nsc.instance);
            }

            nsc.on_update_function(nsc.instance, delta_time);
        });

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

        for (auto [entity] : m_registry.storage<entt::entity>().each()) {
            Entity* actual = &m_entities.at(entity);

            if (actual->forDeletion()) {
                if (actual->hasComponent<NativeScriptComponent>()) {
                    auto nsc = actual->getComponent<NativeScriptComponent>();
                    nsc.on_destroy_function(nsc.instance);
                    nsc.destroy_instance_function();
                }
                m_entities.erase(entity);
                m_registry.destroy(entity);
            }
        }
        
    }

    auto Scene::render() -> void {
        // For every entity with a SpriteComponent, draw it.
        m_registry.view<SpriteComponent>().each([&](const auto& sprite_comp)
        {
            m_render_window.draw(sprite_comp.sprite);
        });
    }

} // namespace rosa