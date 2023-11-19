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

#include "core/Scene.hpp"
#include <graphics/Colour.hpp>
#include <core/GameManager.hpp>
#include <cstddef>
#include <memory>
#include <chrono>
#include <ratio>
#include <core/EventManager.hpp>
#include <spdlog/spdlog.h>
#include <core/SceneSerialiser.hpp>
#include <tracy/Tracy.hpp>
#include <tracy/TracyOpenGL.hpp>

namespace rosa {

    GameManager::GameManager(int window_width, int window_height, const std::string& window_title, bool window_hidden) {

        #if (DEBUG)
        spdlog::set_level(spdlog::level::debug);
        #else
        spdlog::set_level(spdlog::level::info);
        #endif

        try {
            spdlog::info("Setting up OpenGL context");
            m_render_window.init(window_width, window_height, window_title, window_hidden);

            spdlog::info("Initialising resource management");
            [[maybe_unused]]auto& res = ResourceManager::instance();
        
        } catch(Exception& e) {
            spdlog::critical(e.what());
            abort();
        }

        // Setup imgui
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGui::StyleColorsDark();
        ImGui_ImplGlfw_InitForOpenGL(m_render_window.getGlWindowPtr(), true);
        ImGui_ImplOpenGL3_Init("#version 150");

        m_initialised = true;
        spdlog::info("Rosa is up and running!");
    }

    auto GameManager::addScene(const std::string& key, std::unique_ptr<Scene> scene) -> bool {
        ZoneScopedN("Scenes:Add");
        auto [iterator, inserted] = m_scenes.insert_or_assign(key, std::move(scene));
        return inserted;
    }

    auto GameManager::run(int frames) -> void {

        m_frame_count = 0;

        while (m_render_window.isOpen()) {

            if (frames > 0 && m_frame_count >= frames) {
                break;
            }

            std::chrono::time_point<std::chrono::system_clock> now = std::chrono::system_clock::now();
            auto micros = std::chrono::duration_cast<std::chrono::microseconds>(now - m_time);
            float delta_time = static_cast<float>(micros.count()) / 1000000.F;
            m_time = now;

            assert(m_current_scene); // Ensure scene pointer is valid - this needs to be managed internally.

            {
                ZoneScopedNC("Events", 0xFEEF99);
                EventManager::getInstance().pollEvents(m_render_window);

                while (EventManager::getInstance().hasEvents()) {

                    auto event = EventManager::getInstance().popEvent();
                    if (event.type == EventType::EventClose) {
                        m_render_window.close();
                    } else {
                        m_current_scene->input(event);
                    }
                }
            }

            {
                ZoneScopedN("ImGui::NewFrame");
                ImGui_ImplOpenGL3_NewFrame();
                ImGui_ImplGlfw_NewFrame();
                ImGui::NewFrame();
            }

            {
                ZoneScopedNC("Updates", 0x22AA22);
                m_current_scene->update(delta_time);
            }

            {
                ZoneScopedNC("Render", 0xFF0000);

                m_render_window.getFrameBuffer().bind();
                m_render_window.clearColour(m_clear_colour);
                m_current_scene->render();

                {
                    ZoneScopedN("Render::ImGui");
                    ImGui::Render();
                    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
                }

                m_render_window.getFrameBuffer().update();
                m_render_window.getFrameBuffer().unbind();
            }

            auto size = m_render_window.getSize();
            m_render_window.getFrameBuffer().blitColorTo(0, 0, 0, size.x, size.y);
            m_render_window.getFrameBuffer().blitDepthTo(0, 0, 0, size.x, size.y);
        
            //ImGui::SFML::Render(m_render_window);
            m_render_window.display(m_current_scene->m_active_camera_pos);

            FrameMark(nullptr);

            TracyGpuCollect

            m_frame_count++;
        }
    }

    auto GameManager::changeScene(const std::string& key) -> bool {
        ZoneScopedN("Scenes:Change");
        if (auto search = m_scenes.find(key); search != m_scenes.end()) {
            if (m_current_scene != nullptr) {
                m_current_scene->onUnload();
            }
            
            m_current_scene = search->second.get();
            m_current_scene->onLoad();
            return true;
        }

        return false;
    }

    auto GameManager::unpackScene(const std::string& key, const std::string& path) -> bool {
        ZoneScopedN("Scenes:Unpack");
        auto scene = std::make_unique<Scene>(getRenderWindow());
        auto serialiser = SceneSerialiser(*scene.get());
        serialiser.deserialiseFromYaml(path);
        return addScene(key, std::move(scene));
    }

} // namespace rosa