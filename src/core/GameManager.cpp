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

#include "graphics/Colour.hpp"
#include <bits/chrono.h>
#include <core/GameManager.hpp>
#include <cstddef>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <memory>
// #include "imgui.h"
// #include "imgui-SFML.h"
#include <debug/Profiler.hpp>
#include <chrono>
#include <ratio>
#include <vcpkg/buildtrees/imgui/src/v1.89.6-3b94b94077.clean/imgui.h>

namespace rosa {

    GameManager::GameManager() {
        ROSA_PROFILE_SESSION_BEGIN("test");

        spdlog::set_level(spdlog::level::debug);
        spdlog::info("Rosa is up and running!");

        m_render_window.init(window_width, window_height, "SFML Window");

        // [[maybe_unused]] bool imgui_init =  ImGui::SFML::Init(m_render_window);
        // assert(imgui_init);

        /*std::unique_ptr<Scene> new_scene = std::make_unique<Scene>(m_render_window);
        m_scenes.insert_or_assign("test", std::move(new_scene));
        bool change_scene =  changeScene("test");
        assert(change_scene);*/

        [[maybe_unused]]auto& res = ResourceManager::instance();

        // Setup imgui
        IMGUI_CHECKVERSION();
    }

    auto GameManager::instance() -> GameManager& {
        static GameManager s_instance;
        return s_instance;
    }

    GameManager::~GameManager() {
        // if (m_render_window.isOpen()) {
        //     m_render_window.close();
        // }
    }

    auto GameManager::addScene(const std::string& key, std::unique_ptr<Scene> scene) -> bool {
        ROSA_PROFILE_SCOPE("Scenes:add");
        auto [iterator, inserted] = m_scenes.insert_or_assign(key, std::move(scene));
        return inserted;
    }

    auto GameManager::run() -> void {

        ImGui::CreateContext();
        ImGui::StyleColorsDark();
        ImGui_ImplGlfw_InitForOpenGL(m_render_window.getGlWindowPtr(), true);
        ImGui_ImplOpenGL3_Init("#version 150");

        while (m_render_window.isOpen()) {

            std::chrono::time_point<std::chrono::system_clock> now = std::chrono::system_clock::now();
            auto micros = std::chrono::duration_cast<std::chrono::microseconds>(now - m_time);
            float delta_time = micros.count() / 1000000.F;
            m_time = now;

            assert(m_current_scene); // Ensure scene pointer is valid - this needs to be managed internally.

            //sf::Event event{};

            while (m_render_window.pollEvents()) {
                //ImGui::SFML::ProcessEvent(m_render_window, event);

                // if (event.type == sf::Event::Closed) {
                //     m_render_window.close();
                // } else {
                //     m_current_scene->input(event);
                // }
            }

            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            //ImGui::SFML::Update(m_render_window, delta_clock.getElapsedTime());
            //ImGui::ShowDemoWindow();
            m_current_scene->update(delta_time);

            m_render_window.clearColour(Colour{0, 255, 128});

            {
                static float f = 0.0f;
                static int counter = 0;

                ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

                ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
                
                ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
                
                if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
                    counter++;
                ImGui::SameLine();
                ImGui::Text("counter = %d", counter);

                //ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
                ImGui::End();
            }

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