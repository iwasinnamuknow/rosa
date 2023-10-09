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
#include <core/components/LuaScriptComponent.hpp>
#include <functional>
#include <spdlog/spdlog.h>

#include "debug/Profiler.hpp"
#include "imgui.h"
#include "imgui-SFML.h"

namespace rosa {

    Scene::Scene(sf::RenderWindow& render_window) : m_render_window(render_window) { }

    auto Scene::createEntity() -> Entity {
        ROSA_PROFILE_SCOPE("Entity:Create");

        Entity entity{m_registry.create(), std::reference_wrapper<entt::registry>(m_registry)};
        entity.addComponent<TransformComponent>();
        m_entities.insert({entity.getId(), entity});
        return entity;
    }

    auto Scene::create_entity(uuids::uuid uuid) -> Entity {
        ROSA_PROFILE_SCOPE("Entity:Create_UUID");

        Entity entity{uuid, m_registry.create(), std::reference_wrapper<entt::registry>(m_registry)};
        entity.addComponent<TransformComponent>();
        m_entities.insert({entity.getId(), entity});
        return entity;
    }
    
    auto Scene::removeEntity(Entity& entity) -> bool {
        ROSA_PROFILE_SCOPE("Entity:Remove");

        if (m_entities.contains(entity.m_id)) {
            entity.m_for_deletion = true;
            return true;
        }

        return false;
    }

    auto Scene::update(float delta_time) -> void {

#ifdef ROSA_PROFILE
        {
            show_profile_stats();
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

        {
            ROSA_PROFILE_SCOPE("Updates:SpriteTransformUpdate");

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

        // For every entity with a SpriteComponent, draw it.
        m_registry.view<SpriteComponent>().each([&](const auto& sprite_comp)
        {
            m_render_window.draw(sprite_comp.sprite);
        });
    }

    auto Scene::show_profile_stats() -> void {
        bool closeable{true};

        std::vector<rosa::debug::ProfileUiItem> items;

        ImGui::Begin("Profiler", &closeable, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoTitleBar
            | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);

        ImGui::SetWindowPos(ImVec2(0, 0));
        auto s = m_last_frame_time.restart().asSeconds();
        ImGui::Text("FPS: %d", 1 / s);
        ImGui::Text("Frame Time: %dms", s * 1000);

        ImGui::SetCursorPosY(50);
        ImGui::Text("Profiler:");
        
        if (items.size() == 0) {
            auto entries = rosa::debug::Profiler::instance().getLastFrame();
            items.resize(entries.size(), rosa::debug::ProfileUiItem());
            int i{0};

            for (const auto& [key, data] : entries) {
                rosa::debug::ProfileUiItem& item = items[i];
                item.last = data.last_time;
                std::strcpy(item.name, key.c_str());
                item.id = i;
                i++;
            }
        }

        std::sort(items.begin(), items.end(), [](rosa::debug::ProfileUiItem& a, rosa::debug::ProfileUiItem& b) {
            return a.last > b.last;
        });

        ImGui::BeginTable("table1", 2, ImGuiTableFlags_NoHostExtendX | ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_Resizable |
            ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersV );

        ImGui::TableSetupColumn("Function", ImGuiTableColumnFlags_WidthStretch | ImGuiTableColumnFlags_DefaultSort, 3.F, rosa::debug::profile_item_name);
        ImGui::TableSetupColumn("Last", ImGuiTableColumnFlags_WidthStretch, 1.F, rosa::debug::profile_item_last);
        ImGui::TableSetupScrollFreeze(0, 1); // Make row always visible
        ImGui::TableHeadersRow();

        for (const auto& item : items) {
            ImGui::PushID(item.name);
            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            ImGui::TextUnformatted(item.name);
            ImGui::TableNextColumn();
            ImGui::Text("%ldµs", item.last);
            ImGui::PopID();
        }

        ImGui::EndTable();
        ImGui::End();

        rosa::debug::Profiler::instance().clearLastFrame();
    }

} // namespace rosa