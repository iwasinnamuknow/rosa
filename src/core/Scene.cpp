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

#include <ProfilerSections.hpp>
#include <core/Entity.hpp>
#include <ecs/RegistryView.hpp>
#include <graphics/Renderer.hpp>

namespace rosa {

    Scene::Scene(RenderWindow* render_window)
        : m_registry(ecs::EntityRegistry<Entity>()), m_render_window(render_window) {
        ZoneScopedN("Scene:Setup");
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

        ZoneScopedN("Scene:Entity:Create");

        Entity& entity = m_registry.createEntity();
        entity.m_scene = this;
        m_registry.addComponent<TransformComponent>(entity.getUuid());

        return entity;
    }

    auto Scene::createEntity(const Uuid& uuid) -> Entity& {

        ZoneScopedN("Scene:Entity:Create_UUID");

        Entity& entity = m_registry.createEntity(uuid);
        entity.m_scene = this;
        m_registry.addComponent<TransformComponent>(entity.getUuid());

        return entity;
    }

    auto Scene::removeEntity(const Uuid& uuid) -> bool {

        ZoneScopedN("Scene:Entity:Remove");

        // TODO: chase getEntity to return a result instead of throw for non-existant entities

        Entity& entity        = m_registry.getEntity(uuid);
        entity.m_for_deletion = true;

        return true;
    }

    auto Scene::input(const Event& event) -> void {
        {

            ZoneScopedNC("Events:Scene", profiler::detail::tracy_colour_events);
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
            ZoneScopedNC("Events:NativeScript", profiler::detail::tracy_colour_events);

            // Run updates for native script components, instantiating where needed.
            //
            // Removed the fancy RegistryView in favour of a simple indexed loop. This handles
            // cases where a new entity is created, which would have invalidated the iterators.
            //
            // If a new entity is added in this loop, it will be placed at the end of the entity
            // block and the new size will be accounted for on the next iteration.
            //
            // If an entity is deleted in this loop, nothing will be affected as the entity won't
            // be removed until the next frame.
            for (std::size_t i{0}; i < m_registry.count(); i++) {
                auto& ecs_entity = m_registry.getAtIndex(i);
                auto& entity     = m_registry.getEntity(ecs_entity.getUuid());

                if (!entity.isActive()) {
                    continue;
                }

                if (entity.forDeletion()) {
                    continue;
                }

                if (!entity.hasComponent<NativeScriptComponent>()) {
                    continue;
                }

                auto& nsc = entity.getComponent<NativeScriptComponent>();

                if (!nsc.instance) {
                    nsc.instantiate_function(this, &entity);
                    nsc.on_create_function(nsc.instance);
                }

                nsc.on_input_function(nsc.instance, event);
            }
        }

        {
            ZoneScopedNC("Events:LuaScript", profiler::detail::tracy_colour_events);

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
        ZoneScopedNC("Updates", profiler::detail::tracy_colour_updates);

        {
            ZoneScopedNC("Updates:NativeScript", profiler::detail::tracy_colour_updates);

            // See comment above regarding index-based loop
            for (std::size_t i{0}; i < m_registry.count(); i++) {
                auto& ecs_entity = m_registry.getAtIndex(i);
                auto& entity     = m_registry.getEntity(ecs_entity.getUuid());

                if (!entity.isActive()) {
                    continue;
                }

                if (entity.forDeletion()) {
                    continue;
                }

                if (!entity.hasComponent<NativeScriptComponent>()) {
                    continue;
                }

                auto& nsc = entity.getComponent<NativeScriptComponent>();

                if (!nsc.instance) {
                    nsc.instantiate_function(this, &entity);
                    nsc.on_create_function(nsc.instance);
                }

                nsc.on_update_function(nsc.instance, delta_time);
            }
        }

        {
            ZoneScopedNC("Updates:LuaScript", profiler::detail::tracy_colour_updates);

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
            ZoneScopedNC("Updates:TransformUpdate", profiler::detail::tracy_colour_updates);

            // This function only cares about entities with TransformComponent, which is all of them i guess
            for (auto& entity: ecs::RegistryView<Entity, TransformComponent>(m_registry)) {

                Entity* entity_ptr = &entity;

                // orphan
                if (entity.getChildren().empty()) {
                    std::stack<Entity*> stack;
                    stack.push(entity_ptr);

                    while (entity_ptr->getParent() != Uuid()) {
                        entity_ptr = &getEntity(entity_ptr->getParent());
                        stack.push(entity_ptr);
                    }

                    glm::mat4 combined_transform{1.F};
                    while(!stack.empty()) {
                        entity_ptr = stack.top();
                        stack.pop();

                        auto& transform            = entity_ptr->getComponent<TransformComponent>();
                        transform.parent_transform = combined_transform;
                        combined_transform *= transform.getLocalTransform();
                    }
                }
            }
        }

        {
            ZoneScopedNC("Updates:EntitiesForDeletion", profiler::detail::tracy_colour_updates);

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
            ZoneScopedNC("Updates:Camera", profiler::detail::tracy_colour_updates);

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
            ZoneScopedNC("Render:Setup", profiler::detail::tracy_colour_render);
            Renderer::getInstance().clearStats();
            Renderer::getInstance().updateVp(getRenderWindow().getView(), getRenderWindow().getProjection());
        }

        {
            ZoneScopedNC("Render:Sprites", profiler::detail::tracy_colour_render);

            // For every entity with a SpriteComponent, draw it.
            for (const auto& entity: ecs::RegistryView<Entity, SpriteComponent>(m_registry)) {
                auto& sprite_comp = m_registry.getComponent<SpriteComponent>(entity.getUuid());
                auto& transform   = m_registry.getComponent<TransformComponent>(entity.getUuid());
                sprite_comp.draw(transform.getGlobalTransform());
            };
        }

        {
            ZoneScopedNC("Render:Text", profiler::detail::tracy_colour_render);

            // For every entity with a TextComponent, draw it.
            for (const auto& entity: ecs::RegistryView<Entity, TextComponent>(m_registry)) {
                auto& text_comp = m_registry.getComponent<TextComponent>(entity.getUuid());
                auto& transform = m_registry.getComponent<TransformComponent>(entity.getUuid());
                text_comp.render(transform.getGlobalTransform());
            };
        }

        Renderer::getInstance().flushBatch();
    }

    auto Scene::getEntity(const Uuid& uuid) -> Entity& {
        return m_registry.getEntity(uuid);
    }


} // namespace rosa