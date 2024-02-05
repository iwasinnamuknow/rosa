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
#include <core/components/CameraComponent.hpp>
#include <core/components/LuaScriptComponent.hpp>
#include <core/components/MusicPlayerComponent.hpp>
#include <core/components/NativeScriptComponent.hpp>
#include <core/components/SoundPlayerComponent.hpp>
#include <core/components/SpriteComponent.hpp>
#include <core/components/TextComponent.hpp>
#include <core/components/TransformComponent.hpp>
#include <functional>
#include <stack>

#include <core/Entity.hpp>
#include <ecs/RegistryView.hpp>
#include <graphics/Renderer.hpp>

namespace rosa {

    Scene::Scene(RenderWindow* render_window)
        : m_registry(ecs::EntityRegistry<Entity>()), m_render_window(render_window) {
        m_registry.registerComponent<TransformComponent>();
        m_registry.registerComponent<SpriteComponent>();
        m_registry.registerComponent<NativeScriptComponent>();
        m_registry.registerComponent<LuaScriptComponent>();
        m_registry.registerComponent<CameraComponent>();
        m_registry.registerComponent<TextComponent>();
        m_registry.registerComponent<SoundPlayerComponent>();
        m_registry.registerComponent<MusicPlayerComponent>();
    }

    auto Scene::createEntity() -> Entity& {
        
        ZoneScopedN("Entity:Create");

        Entity& entity = m_registry.createEntity();
        entity.m_scene = this;
        m_registry.addComponent<TransformComponent>(entity.getUuid());

        return entity;
    }

    auto Scene::createEntity(Uuid uuid) -> Entity& {

        ZoneScopedN("Entity:Create_UUID");

        Entity& entity = m_registry.createEntity(uuid);
        entity.m_scene = this;
        m_registry.addComponent<TransformComponent>(entity.getUuid());

        return entity;
    }
    
    auto Scene::removeEntity(Uuid uuid) -> bool {

        ZoneScopedN("Entity:Remove");

        // TODO: chase getEntity to return a result instead of throw for non-existant entities

        Entity& entity        = m_registry.getEntity(uuid);
        entity.m_for_deletion = true;

        return true;
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
            for (auto& entity: ecs::RegistryView<Entity, NativeScriptComponent>(m_registry)) {
                auto& nsc = m_registry.getComponent<NativeScriptComponent>(entity.getUuid());

                if (!nsc.instance) {
                    nsc.instantiate_function(this, &entity);
                    nsc.on_create_function(nsc.instance);
                }

                nsc.on_input_function(nsc.instance, event);
            }
        }

        {
            ZoneScopedN("Events:LuaScript");

            // Run updates for native script components, instantiating where needed
            for (auto& entity: ecs::RegistryView<Entity, LuaScriptComponent>(m_registry)) {

                auto& lsc = m_registry.getComponent<LuaScriptComponent>(entity.getUuid());

                //Entity* actual = &m_entities.at(entity);

                auto result = lsc.m_on_input_function(Event(event));
                if (!result.valid()) {
                    sol::error err = result;
                    spdlog::error("Failed to call onInput for lua script: {}", err.what());
                }
            }
        }
    }

    auto Scene::update(float delta_time) -> void {

        m_last_frame_time = static_cast<double>(delta_time);

        {
            ZoneScopedN("Updates:NativeScript");

            // Run updates for native script components, instantiating where needed
            for (auto& entity: ecs::RegistryView<Entity, NativeScriptComponent>(m_registry)) {

                auto& nsc = m_registry.getComponent<NativeScriptComponent>(entity.getUuid());

                if (!nsc.instance) {
                    nsc.instantiate_function(this, &entity);
                    nsc.on_create_function(nsc.instance);
                }

                nsc.on_update_function(nsc.instance, delta_time);
            }
        }

        {
            ZoneScopedN("Updates:LuaScript");

            // Run updates for lua script components
            for (auto& entity: ecs::RegistryView<Entity, LuaScriptComponent>(m_registry)) {

                auto& lsc    = m_registry.getComponent<LuaScriptComponent>(entity.getUuid());
                auto result = lsc.m_on_update_function(delta_time);
                if (!result.valid()) {
                    sol::error err = result;
                    spdlog::error("Failed to call onUpdate for lua script: {}", err.what());
                }
            }
        }

        {
            ZoneScopedN("Updates:TransformUpdate");

            // This function only cares about entities with TransformComponent, which is all of them i guess
            for (auto& entity: ecs::RegistryView<Entity, TransformComponent>(m_registry)) {

                Entity* entity_ptr = &entity;

                if (entity.m_children.empty()) {
                    std::stack<Entity*> stack;
                    stack.push(entity_ptr);

                    while (entity.m_parent != Uuid()) {
                        entity_ptr = &getEntity(entity.getParent());
                        stack.push(entity_ptr);
                    }

                    glm::mat4 combined_transform{1.F};
                    while(!stack.empty()) {
                        entity_ptr = stack.top();
                        stack.pop();

                        auto& transform            = entity.getComponent<TransformComponent>();
                        transform.parent_transform = combined_transform;
                        combined_transform *= transform.getLocalTransform();
                    }
                }
            }
        }

        {
            ZoneScopedN("Updates:EntitiesForDeletion");

            for (auto& entity: ecs::RegistryView<Entity>(m_registry)) {

                if (entity.forDeletion()) {
                    // TODO move this stuff to removeEntity above
                    if (m_registry.hasComponent<NativeScriptComponent>(entity.getUuid())) {
                        auto& nsc = m_registry.getComponent<NativeScriptComponent>(entity.getUuid());
                        nsc.on_destroy_function(nsc.instance);
                        nsc.destroy_instance_function();
                    } else if (m_registry.hasComponent<LuaScriptComponent>(entity.getUuid())) {
                        auto& lsc = m_registry.getComponent<LuaScriptComponent>(entity.getUuid());
                        lsc.m_on_delete_function();
                    }
                    m_registry.removeEntity(entity.getUuid());
                }
            }
        }

        {
            ZoneScopedN("Updates:Camera");

            bool found_active{false};
            for (auto& entity: ecs::RegistryView<Entity, CameraComponent>(m_registry)) {
                if (found_active) {
                    break;
                }

                auto& cam       = m_registry.getComponent<CameraComponent>(entity.getUuid());
                auto& transform = m_registry.getComponent<TransformComponent>(entity.getUuid());

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

            // For every entity with a SpriteComponent, draw it.
            for (const auto& entity: ecs::RegistryView<Entity, SpriteComponent>(m_registry)) {
                auto& sprite_comp = m_registry.getComponent<SpriteComponent>(entity.getUuid());
                auto& transform   = m_registry.getComponent<TransformComponent>(entity.getUuid());
                //auto sprite_comp = m_registry.get<SpriteComponent>(entid);
                sprite_comp.draw(transform.getGlobalTransform());
            };
        }

        //Renderer::getInstance().flushBatch();

        {
            ZoneScopedN("Render:Text");

            // For every entity with a SpriteComponent, draw it.
            for (const auto& entity: ecs::RegistryView<Entity, TextComponent>(m_registry)) {
                auto& text_comp = m_registry.getComponent<TextComponent>(entity.getUuid());
                auto& transform = m_registry.getComponent<TransformComponent>(entity.getUuid());
                //auto sprite_comp = m_registry.get<SpriteComponent>(entid);
                text_comp.render(transform.getGlobalTransform());
            };
        }

        Renderer::getInstance().flushBatch();
    }

    auto Scene::getEntity(Uuid uuid) -> Entity& {
        return m_registry.getEntity(uuid);
    }


} // namespace rosa