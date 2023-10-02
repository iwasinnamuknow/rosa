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
#include <TestScript.hpp>
#include <entt/entity/fwd.hpp>

#include "imgui.h"
#include "imgui-SFML.h"

namespace rosa {

    Scene::Scene(sf::RenderWindow& render_window) : m_render_window(render_window) {
        auto new_entity = createEntity();
        new_entity.addComponent<SpriteComponent>();

        getRegistry().emplace<NativeScriptComponent>(new_entity).bind<TestScript>();
    }

    auto Scene::createEntity() -> Entity {
        Entity entity{m_registry.create(), &m_registry};
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
                nsc.instantiate_function(this, actual);
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

    auto Scene::render(sf::RenderWindow& window) -> void {
        // For every entity with a SpriteComponent, draw it.
        m_registry.view<SpriteComponent>().each([&](const auto& sprite_comp)
        {
            window.draw(sprite_comp.sprite);
        });
    }

} // namespace rosa