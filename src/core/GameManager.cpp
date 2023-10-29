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

#include <graphics/Colour.hpp>
#include <bits/chrono.h>
#include <core/GameManager.hpp>
#include <cstddef>
#include <memory>
#include <debug/Profiler.hpp>
#include <chrono>
#include <ratio>
#include <core/EventManager.hpp>
#include <spdlog/spdlog.h>

namespace rosa {

    GameManager::GameManager() {
        ROSA_PROFILE_SESSION_BEGIN("test");

        #if (DEBUG)
        spdlog::set_level(spdlog::level::debug);
        #else
        spdlog::set_level(spdlog::level::info);
        #endif

        try {
            spdlog::info("Setting up OpenGL context");
            m_render_window.init(window_width, window_height, "SFML Window");

            spdlog::info("Initialising resource management");
            [[maybe_unused]]auto& res = ResourceManager::instance();
        
        } catch(Exception& e) {
            spdlog::critical(e.what());
            abort();
        }

        spdlog::info("Rosa is up and running!");
    }

    auto GameManager::instance() -> GameManager& {
        static GameManager s_instance;
        return s_instance;
    }

    auto GameManager::addScene(const std::string& key, std::unique_ptr<Scene> scene) -> bool {
        ROSA_PROFILE_SCOPE("Scenes:add");
        auto [iterator, inserted] = m_scenes.insert_or_assign(key, std::move(scene));
        return inserted;
    }

    auto GameManager::run() -> void {

        // Setup imgui
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGui::StyleColorsDark();
        ImGui_ImplGlfw_InitForOpenGL(m_render_window.getGlWindowPtr(), true);
        ImGui_ImplOpenGL3_Init("#version 150");

        while (m_render_window.isOpen()) {

            std::chrono::time_point<std::chrono::system_clock> now = std::chrono::system_clock::now();
            auto micros = std::chrono::duration_cast<std::chrono::microseconds>(now - m_time);
            float delta_time = static_cast<float>(micros.count()) / 1000000.F;
            m_time = now;

            assert(m_current_scene); // Ensure scene pointer is valid - this needs to be managed internally.

            {
                ROSA_PROFILE_SCOPE("DispatchEvents");
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

            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            m_current_scene->update(delta_time);

            m_render_window.clearColour(m_clear_colour);

            m_current_scene->render();

            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

            //ImGui::SFML::Render(m_render_window);
            m_render_window.display();
        }

        ROSA_PROFILE_SESSION_END();
    }

    auto GameManager::changeScene(const std::string& key) -> bool {
        ROSA_PROFILE_SCOPE("Scenes:change");
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

} // namespace rosa