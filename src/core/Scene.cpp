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

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <graphics/gl.hpp>
#include <GLFW/glfw3.h>

#include <core/ResourceManager.hpp>
#include <core/Scene.hpp>
#include <core/components/SpriteComponent.hpp>
#include <core/components/TransformComponent.hpp>
#include <core/components/NativeScriptComponent.hpp>
#include <core/components/LuaScriptComponent.hpp>
#include <functional>

namespace rosa {

    Scene::Scene(RenderWindow& render_window) : m_render_window(render_window) { }

    auto Scene::createEntity() -> Entity& {
        ROSA_PROFILE_SCOPE("Entity:Create");

        Entity entity{m_registry.create(), std::reference_wrapper<entt::registry>(m_registry)};
        entity.addComponent<TransformComponent>();
        m_entities.insert({entity.getId(), entity});
        return m_entities.at(entity.getId());
    }

    auto Scene::create_entity(uuids::uuid uuid) -> Entity& {
        ROSA_PROFILE_SCOPE("Entity:Create_UUID");

        Entity entity{uuid, m_registry.create(), std::reference_wrapper<entt::registry>(m_registry)};
        entity.addComponent<TransformComponent>();
        m_entities.insert({entity.getId(), entity});
        return m_entities.at(entity.getId());
    }
    
    auto Scene::removeEntity(Entity& entity) -> bool {
        ROSA_PROFILE_SCOPE("Entity:Remove");

        if (m_entities.contains(entity.m_id)) {
            entity.m_for_deletion = true;
            return true;
        }

        return false;
    }

    auto Scene::input(const Event& event) -> void {
        {
            ROSA_PROFILE_SCOPE("Events:Scene");
            switch (event.keyboard.type) {
                case KeyboardEventType::KeyReleased:
                    if (event.keyboard.key == KeyF12) {
                        m_show_profile_stats = !m_show_profile_stats;
                    } else if (event.keyboard.key == KeyEscape) {
                        m_render_window.close();
                    }
                    break;
                case KeyboardEventType::KeyPressed:
                case KeyboardEventType::KeyRepeated:
                    break;
            }
        }

        {
            ROSA_PROFILE_SCOPE("Events:NativeScript");

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
            ROSA_PROFILE_SCOPE("Events:LuaScript");

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

#ifdef ROSA_PROFILE
        {
            if (m_show_profile_stats) {
                show_profile_stats(&m_show_profile_stats);
            }
        }
#endif // ROSA_PROFILE

        {
            ROSA_PROFILE_SCOPE("Updates:NativeScript");

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
            ROSA_PROFILE_SCOPE("Updates:LuaScript");

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

        // {
        //     ROSA_PROFILE_SCOPE("Updates:SpriteTransformUpdate");

        //     // This function only cares about entities with SpriteComponent and TransformComponent
        //     auto view = m_registry.view<SpriteComponent, TransformComponent>();

        //     for (const auto& entid : view)
        //     {
        //         TransformComponent& transform = m_registry.get<TransformComponent>(entid);
        //         SpriteComponent& sprite_comp = m_registry.get<SpriteComponent>(entid);

        //         // Entity* entity = &m_entities.at(entid);

        //         // if (entity->m_parent == entt::null) {
        //         //     //
        //         //     // turn it on its head, work top down, one layer at a time
        //         //     //
        //         //     // A -> B -> C, no accumulation
        //         //     //
        //         //     auto p_transform = m_entities.at(entity->getParent()).getComponent<TransformComponent>();
                    
        //         //     auto angle = -(transform.rotation * (std::numbers::pi / 180));
        //         //     const float cosine = std::cos(angle);
        //         //     const float sine   = std::sin(angle);
        //         //     const float sxc    = transform.scale.x * cosine;
        //         //     const float syc    = transform.scale.y * cosine;
        //         //     const float sxs    = transform.scale.x * sine;
        //         //     const float sys    = transform.scale.y * sine;
        //         //     const float tx     = -p_transform.position.x * sxc - p_transform.position.y * sys + transform.position.x;
        //         //     const float ty     = p_transform.position.x * sxs - p_transform.position.y * syc + transform.position.y;

        //         //     // parent_pos += p_transform.position;
        //         //     parent_rot += p_transform.rotation;
                    
        //         //     entity = &m_entities.at(entity->getParent());
        //         // }

        //         //auto updated_pos = transform.position + parent_pos;

        //         // todo take parents position
        //         // sprite_comp.setPosition(transform.position);
        //         // sprite_comp.setScale(transform.scale);
        //         // sprite_comp.setRotation(transform.rotation);
        //     }
        // }

        {
            ROSA_PROFILE_SCOPE("Updates:EntitiesForDeletion");

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
                    m_registry.destroy(entity);
                }
            }
        }
        
    }

    auto Scene::render() -> void {
        ROSA_PROFILE_SCOPE("Render:Sprites");

        auto view = m_registry.view<SpriteComponent>();

        // For every entity with a SpriteComponent, draw it.
        for (const auto& entid : view)
        {
            auto sprite_comp = m_registry.get<SpriteComponent>(entid);
            auto transform = m_registry.get<TransformComponent>(entid);
            //auto sprite_comp = m_registry.get<SpriteComponent>(entid);
            m_render_window.draw(sprite_comp, transform.getLocalTransform());
        };
    }

    auto Scene::show_profile_stats(bool* open) const -> void {

        std::vector<rosa::debug::ProfileUiItem> items;

        ImGui::Begin("Profiler", open, static_cast<ImGuiWindowFlags>(
            static_cast<unsigned int>(ImGuiWindowFlags_NoCollapse) | static_cast<unsigned int>(ImGuiWindowFlags_NoSavedSettings) | 
            static_cast<unsigned int>(ImGuiWindowFlags_NoMove) | static_cast<unsigned int>(ImGuiWindowFlags_NoResize)
        ));

        double fps = 1.0 / m_last_frame_time;

        ImGui::SetWindowPos(ImVec2(0, 0));
        ImGui::Text("FPS: %f", fps);
        ImGui::Text("Frame Time: %dms", static_cast<int>(m_last_frame_time * 1000));
        
        if (items.empty()) {
            auto entries = rosa::debug::Profiler::instance().getLastFrame();
            items.resize(entries.size(), rosa::debug::ProfileUiItem());
            int index{0};

            for (const auto& [key, data] : entries) {
                rosa::debug::ProfileUiItem& item = items[index];
                item.last = data.last_time;
                item.name = key;
                item.id = index;
                index++;
            }
        }

        std::sort(items.begin(), items.end(), [](rosa::debug::ProfileUiItem& first, rosa::debug::ProfileUiItem& second) {
            return first.last > second.last;
        });

        ImGui::BeginTable("table1", 2, static_cast<ImGuiWindowFlags>(
            static_cast<unsigned int>(ImGuiTableFlags_NoHostExtendX) | static_cast<unsigned int>(ImGuiTableFlags_SizingFixedFit) | 
            static_cast<unsigned int>(ImGuiTableFlags_Resizable) | static_cast<unsigned int>(ImGuiTableFlags_BordersOuter) | 
            static_cast<unsigned int>(ImGuiTableFlags_BordersV)
        ));

        ImGui::TableSetupColumn("Function", ImGuiTableColumnFlags_WidthStretch | ImGuiTableColumnFlags_DefaultSort, 3.F, rosa::debug::profile_item_name);
        ImGui::TableSetupColumn("Last", ImGuiTableColumnFlags_WidthStretch, 1.F, rosa::debug::profile_item_last);
        ImGui::TableSetupScrollFreeze(0, 1); // Make row always visible
        ImGui::TableHeadersRow();

        for (const auto& item : items) {
            ImGui::PushID(item.name.c_str());
            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            ImGui::TextUnformatted(item.name.c_str());
            ImGui::TableNextColumn();
            ImGui::Text("%ldµs", item.last);
            ImGui::PopID();
        }

        ImGui::EndTable();
        ImGui::End();

        rosa::debug::Profiler::instance().clearLastFrame();
    }

} // namespace rosa