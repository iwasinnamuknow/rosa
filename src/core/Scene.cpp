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
 *  You should have received a copy of the GNU General Public License along with rosa. If not,
 *  see <https://www.gnu.org/licenses/>.
 */

#include <chrono>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <graphics/gl.hpp>
#include <GLFW/glfw3.h>
#include <tracy/Tracy.hpp>

#include <core/ResourceManager.hpp>
#include <core/Scene.hpp>
#include <core/components/SpriteComponent.hpp>
#include <core/components/TransformComponent.hpp>
#include <core/components/NativeScriptComponent.hpp>
#include <core/components/LuaScriptComponent.hpp>
#include <core/components/CameraComponent.hpp>
#include <core/components/TextComponent.hpp>
#include <functional>
#include <stack>

#include <core/Entity.hpp>
#include <graphics/Renderer.hpp>

namespace rosa {

    Scene::Scene(RenderWindow* render_window) : m_render_window(render_window) { }

    auto Scene::createEntity() -> Entity& {
        
        ZoneScopedN("Entity:Create");

        Entity entity{m_registry.create(), *this};
        entity.addComponent<TransformComponent>();
        m_entities.insert({entity.getId(), entity});
        m_uuid_to_entity.insert({entity.getUUID(), entity.getId()});
        m_entity_to_uuid.insert({entity.getId(), entity.getUUID()});
        return m_entities.at(entity.getId());
    }

    auto Scene::createEntity(Uuid uuid) -> Entity& {

        ZoneScopedN("Entity:Create_UUID");

        Entity entity{uuid, m_registry.create(), *this};
        entity.addComponent<TransformComponent>();
        m_entities.insert({entity.getId(), entity});
        m_uuid_to_entity.insert({entity.getUUID(), entity.getId()});
        m_entity_to_uuid.insert({entity.getId(), entity.getUUID()});
        return m_entities.at(entity.getId());
    }
    
    auto Scene::removeEntity(Uuid uuid) -> bool {
        
        ZoneScopedN("Entity:Remove");

        auto ent_id = m_uuid_to_entity.at(uuid);

        if (m_entities.contains(ent_id)) {
            auto& entity = m_entities.at(ent_id);
            entity.m_for_deletion = true;
            return true;
        }

        return false;
    }

    auto Scene::input(const Event& event) -> void {
        {
            
            ZoneScopedN("Events:Scene");
            switch (event.keyboard.type) {
                case KeyboardEventType::KeyReleased:
                    if (event.keyboard.key == KeyEscape) {
                        getRenderWindow().close();
                    }
                    break;
                case KeyboardEventType::KeyPressed:
                case KeyboardEventType::KeyRepeated:
                    break;
            }
        }

        {
            ZoneScopedN("Events:NativeScript");

            // Run updates for native script components, instantiating where needed
            m_registry.view<NativeScriptComponent>().each([this, event](entt::entity entity, auto& nsc) {

                Entity* actual = &m_entities.at(entity);

                if (!nsc.instance) {
                    nsc.instantiate_function(std::reference_wrapper<Scene>(*this), std::reference_wrapper<Entity>(*actual));
                    nsc.on_create_function(nsc.instance);
                }

                nsc.on_input_function(nsc.instance, event);
            });
        }

        {
            ZoneScopedN("Events:LuaScript");

            // Run updates for native script components, instantiating where needed
            m_registry.view<LuaScriptComponent>().each([event](auto& lsc) {

                //Entity* actual = &m_entities.at(entity);

                auto result = lsc.m_on_input_function(Event(event));
                if (!result.valid()) {
                    sol::error err = result;
                    spdlog::error("Failed to call onInput for lua script: {}", err.what());
                }
            });
        }
    }

    auto Scene::update(float delta_time) -> void {

        m_last_frame_time = static_cast<double>(delta_time);

        {
            ZoneScopedN("Updates:NativeScript");

            // Run updates for native script components, instantiating where needed
            m_registry.view<NativeScriptComponent>().each([this, delta_time](entt::entity entity, auto& nsc) {

                Entity* actual = &m_entities.at(entity);

                if (!nsc.instance) {
                    nsc.instantiate_function(std::reference_wrapper<Scene>(*this), std::reference_wrapper<Entity>(*actual));
                    nsc.on_create_function(nsc.instance);
                }

                nsc.on_update_function(nsc.instance, delta_time);
            });
        }

        {
            ZoneScopedN("Updates:LuaScript");

            // Run updates for lua script components
            m_registry.view<LuaScriptComponent>().each([delta_time](entt::entity /*entity*/, LuaScriptComponent& lsc) {

    //            Entity* actual = &m_entities.at(entity);
                auto result = lsc.m_on_update_function(delta_time);
                if (!result.valid()) {
                    sol::error err = result;
                    spdlog::error("Failed to call onUpdate for lua script: {}", err.what());
                }
            });
        }

        {
            ZoneScopedN("Updates:TransformUpdate");

            // This function only cares about entities with TransformComponent, which is all of them i guess
            auto view = m_registry.view<TransformComponent>();

            for (const auto& entid : view)
            {
                Entity* entity = &m_entities.at(entid);

                if (entity->m_children.empty()) {
                    std::stack<Entity*> stack;

                    
                    stack.push(entity);

                    while (entity->m_parent != Uuid()) {
                        entity = &m_entities.at(m_uuid_to_entity.at(entity->m_parent));
                        stack.push(entity);
                    }

                    glm::mat4 combined_transform{1.F};
                    while(!stack.empty()) {
                        entity = stack.top();
                        stack.pop();

                        auto& transform = entity->getComponent<TransformComponent>();
                        transform.parent_transform = combined_transform;
                        combined_transform *= transform.getLocalTransform();
                    }
                }
            }
        }

        {
            ZoneScopedN("Updates:EntitiesForDeletion");

            for (auto [entity] : m_registry.storage<entt::entity>().each()) {
                Entity* actual = &m_entities.at(entity);

                if (actual->forDeletion()) {
                    // TODO move this stuff to removeEntity above
                    if (actual->hasComponent<NativeScriptComponent>()) {
                        auto& nsc = actual->getComponent<NativeScriptComponent>();
                        nsc.on_destroy_function(nsc.instance);
                        nsc.destroy_instance_function();
                    } else if (actual->hasComponent<LuaScriptComponent>()) {
                        auto& lsc = actual->getComponent<LuaScriptComponent>();
                        lsc.m_on_delete_function();
                        //lua_close(lsc.m_state);
                    }
                    m_entities.erase(entity);
                    m_uuid_to_entity.erase(actual->getUUID());
                    m_entity_to_uuid.erase(entity);
                    m_registry.destroy(entity);
                }
            }
        }

        {
            ZoneScopedN("Updates:Camera");

            bool found_active{false};
            auto view = m_registry.view<CameraComponent>();

            for (const auto& entid : view)
            {
                if (found_active) {
                    break;
                }

                auto& entity = m_entities.at(entid);
                auto& cam = entity.getComponent<CameraComponent>();
                auto& transform = entity.getComponent<TransformComponent>();

                if (cam.getEnabled()) {
                    found_active = true;
                    auto global_transform = transform.getGlobalTransform();
                    m_active_camera_pos = global_transform * glm::vec4(1, 1, 1, 1);
                }
            }
        }
        
    }

    auto Scene::render() -> void {

        if (m_render_window == nullptr) {
            spdlog::critical("Rendering disabled. Running in test mode?");
            return;
        }

        {
            ZoneScopedN("Render:BatchRenderer:Setup");
            Renderer::getInstance().clearStats();
            Renderer::getInstance().updateVp(getRenderWindow().getView(), getRenderWindow().getProjection());
        }

        {
            ZoneScopedN("Render:Sprites");

            auto view = m_registry.view<SpriteComponent>();

            // For every entity with a SpriteComponent, draw it.
            for (const auto& entid : view)
            {
                auto& sprite_comp = m_registry.get<SpriteComponent>(entid);
                auto& transform = m_registry.get<TransformComponent>(entid);
                //auto sprite_comp = m_registry.get<SpriteComponent>(entid);
                sprite_comp.draw(transform.getGlobalTransform());
            };
        }

        //Renderer::getInstance().flushBatch();

        {
            ZoneScopedN("Render:Text");

            auto view = m_registry.view<TextComponent>();

            // For every entity with a SpriteComponent, draw it.
            for (const auto& entid : view)
            {
                auto& text_comp = m_registry.get<TextComponent>(entid);
                auto& transform = m_registry.get<TransformComponent>(entid);
                //auto sprite_comp = m_registry.get<SpriteComponent>(entid);
                text_comp.render(transform.getGlobalTransform());
            };
        }

        Renderer::getInstance().flushBatch();
    }

} // namespace rosa