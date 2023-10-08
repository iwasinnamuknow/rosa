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

#include <core/GameManager.hpp>
#include <cstddef>
#include <memory>
#include "imgui.h"
#include "imgui-SFML.h"
#include <debug/Profiler.hpp>

namespace rosa {

    GameManager::GameManager() {
        ROSA_PROFILE_SESSION_BEGIN("test");

        spdlog::set_level(spdlog::level::debug);
        spdlog::info("Rosa is up and running!");

        m_render_window.create(sf::VideoMode(window_width, window_height), "SFML Window", sf::Style::None);

        [[maybe_unused]] bool imgui_init =  ImGui::SFML::Init(m_render_window);
        assert(imgui_init);

        /*std::unique_ptr<Scene> new_scene = std::make_unique<Scene>(m_render_window);
        m_scenes.insert_or_assign("test", std::move(new_scene));
        bool change_scene =  changeScene("test");
        assert(change_scene);*/

        [[maybe_unused]]auto& res = ResourceManager::instance();
    }

    auto GameManager::instance() -> GameManager& {
        static GameManager s_instance;
        return s_instance;
    }

    GameManager::~GameManager() {
        if (m_render_window.isOpen()) {
            m_render_window.close();
        }
    }

    auto GameManager::currentScene() -> Scene& {
        return *m_current_scene;
    }

    auto GameManager::addScene(const std::string& key, std::unique_ptr<Scene> scene) -> bool {
        auto [iterator, inserted] = m_scenes.insert_or_assign(key, std::move(scene));
        return inserted;
    }

    auto GameManager::run() -> void {
        sf::Clock delta_clock;

        while (m_render_window.isOpen()) {
            sf::Event event{};

            while (m_render_window.pollEvent(event)) {
                ImGui::SFML::ProcessEvent(m_render_window, event);

                if (event.type == sf::Event::Closed) {
                    m_render_window.close();
                }
            }

            assert(m_current_scene); // Ensure scene pointer is valid - this needs to be managed internally.

            ImGui::SFML::Update(m_render_window, delta_clock.getElapsedTime());
            //ImGui::ShowDemoWindow();
            m_current_scene->update(delta_clock.restart().asSeconds());

            m_render_window.clear();
            m_current_scene->render();

            ImGui::SFML::Render(m_render_window);
            m_render_window.display();

        }

        ROSA_PROFILE_SESSION_END();
    }

    auto GameManager::changeScene(const std::string& key) -> bool {
        if (auto search = m_scenes.find(key); search != m_scenes.end()) {
            m_current_scene = search->second.get();
            return true;
        }

        return false;
    }

} // namespace rosa