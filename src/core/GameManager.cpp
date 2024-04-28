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

#include <ProfilerSections.hpp>
#include <chrono>
#include <core/EventManager.hpp>
#include <core/GameManager.hpp>
#include <core/Scene.hpp>
#include <core/SceneSerialiser.hpp>
#include <memory>
#include <spdlog/spdlog.h>
#include <tracy/Tracy.hpp>
#include <tracy/TracyOpenGL.hpp>

namespace rosa {

    GameManager::GameManager(int window_width, int window_height, const std::string &window_title, int msaa,
                             bool window_hidden) {

#if (DEBUG)
        spdlog::set_level(spdlog::level::debug);
#else
        spdlog::set_level(spdlog::level::info);
#endif

        try {
            m_render_window = std::make_unique<RenderWindow>(window_width, window_height, window_title, msaa, window_hidden);

            spdlog::info("Initialising resource management");
            [[maybe_unused]]auto &res = ResourceManager::getInstance();

        } catch (Exception &e) {
            spdlog::critical(e.what());
            abort();
        }

        // Setup imgui
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGui::StyleColorsDark();
        ImGui_ImplGlfw_InitForOpenGL(m_render_window->getGlWindowPtr(), true);
        ImGui_ImplOpenGL3_Init("#version 150");

        spdlog::info("Rosa is up and running!");
    }

    GameManager::~GameManager() {
        spdlog::info("Rosa is shutting down");

        // make sure scene destructors are called before everything else
        m_scenes.clear();

        ImGui_ImplGlfw_Shutdown();
        ImGui_ImplOpenGL3_Shutdown();
        ImGui::DestroyContext();
        Renderer::shutdown();
        ResourceManager::shutdown();
    }

    auto GameManager::addScene(const std::string &key, std::unique_ptr<Scene> scene) -> bool {
        ZoneScopedN("Scenes:Add");
        auto [iterator, inserted] = m_scenes.insert_or_assign(key, std::move(scene));
        return inserted;
    }

    auto GameManager::run(std::uint64_t frames) -> void {

        m_frame_count = 0;

        while (m_render_window->isOpen()) {

            ZoneScopedN("FrameLoop");

            if (frames > 0 && m_frame_count >= frames) {
                break;
            }

            std::chrono::time_point<std::chrono::system_clock> now = std::chrono::system_clock::now();
            auto micros = std::chrono::duration_cast<std::chrono::microseconds>(now - m_time);
            float delta_time = static_cast<float>(micros.count()) / 1000000.F;
            m_time = now;

            assert(m_current_scene); // Ensure scene pointer is valid - this needs to be managed internally.

            {
                ZoneScopedNC("ImGui::NewFrame", profiler::detail::tracy_colour_imgui);
                ImGui_ImplOpenGL3_NewFrame();
                ImGui_ImplGlfw_NewFrame();
                ImGui::NewFrame();

                drawRenderStats(delta_time);

                //ImGui::ShowDemoWindow(nullptr);
            }

            {
                m_current_scene->update(delta_time);
            }

            {
                ZoneScopedNC("Events", profiler::detail::tracy_colour_events);
                EventManager::getInstance().pollEvents(*m_render_window);

                while (EventManager::getInstance().hasEvents()) {

                    auto event = EventManager::getInstance().popEvent();
                    if (event.type == EventType::EventClose) {
                        m_render_window->close();
                    } else {
                        if (event.type == EventType::EventKeyboard) {
                            if (event.keyboard.key == Key::KeyF10 && event.keyboard.type == KeyboardEventType::KeyReleased) {
                                m_show_renderer_stats = !m_show_renderer_stats;
                            }
                        }
                        m_current_scene->input(event);
                    }
                }
            }

            {
                ZoneScopedNC("Render", profiler::detail::tracy_colour_render);

                m_render_window->getFrameBuffer().bind();
                m_render_window->clearWindow(m_clear_colour);
                m_current_scene->render();

                {
                    ZoneScopedNC("Render::ImGui", profiler::detail::tracy_colour_imgui);
                    ImGui::Render();
                    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
                }

                m_render_window->getFrameBuffer().update();
                m_render_window->getFrameBuffer().unbind();

                auto size = m_render_window->getWindowSize();
                assert(m_render_window->getFrameBuffer().getWidth() == size.x);
                m_render_window->getFrameBuffer().blitColorTo(0, 0, 0, static_cast<int>(size.x), static_cast<int>(size.y));
                m_render_window->getFrameBuffer().blitDepthTo(0, 0, 0, static_cast<int>(size.x), static_cast<int>(size.y));

                m_render_window->display(m_current_scene->m_active_camera_pos);
            }

#ifdef TRACY_ENABLE
            FrameMark;
#endif

            TracyGpuCollect

            m_frame_count++;
        }
    }

    auto GameManager::changeScene(const std::string &key) -> bool {
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

    auto GameManager::unpackScene(const std::string &key, const std::string &path) -> bool {
        ZoneScopedN("Scenes:Unpack");
        auto scene = std::make_unique<Scene>(getRenderWindow());
        auto serialiser = SceneSerialiser(*scene.get());
        serialiser.deserialiseFromYaml(path);
        return addScene(key, std::move(scene));
    }

    void GameManager::drawRenderStats(float delta_time) {

        if (m_renderer_stats.size() >= 120) {
            m_renderer_stats.pop_back();
        }

        if (m_frame_times.size() >= 120) {
            m_frame_times.pop_back();
        }

        RendererStats stats = Renderer::getInstance().getStats();
        m_renderer_stats.push_front(stats);
        m_frame_times.push_front(delta_time);

        if (m_show_renderer_stats) {
            ImGui::SetNextWindowPos(ImVec2(0, 0));
            ImGui::SetNextWindowSize(ImVec2(302, 500));
            ImGui::Begin("Renderer Stats", nullptr,
                         ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoBackground);

            std::array<float, 120> draw_calls{0};
            std::array<float, 120> vertices{0};
            std::array<float, 120> textures{0};
            std::array<float, 120> shaders{0};
            std::array<float, 120> frame_times{0};

            {
                std::size_t index{0};
                for (const auto& time: m_frame_times) {
                    auto fps           = 1.F / time;
                    frame_times[index] = fps;
                    index++;
                }
            }

            {
                std::size_t index{0};
                for (const auto& entry: m_renderer_stats) {

                    draw_calls[index] = static_cast<float>(entry.draws);
                    vertices[index]   = static_cast<float>(entry.vertices);
                    textures[index]   = static_cast<float>(entry.textures);
                    shaders[index]    = static_cast<float>(entry.shaders);

                    index++;
                }
            }

            ImGui::Text("Framerate %.1f", 1.F / delta_time);
            ImGui::PlotLines("", frame_times.data(), 120, 0, nullptr, 0.F, 1.F, ImVec2(300.F, 50.F));
            ImGui::NewLine();

            ImGui::Text("Draw Calls %d", stats.draws);
            ImGui::PlotLines("", draw_calls.data(), 120, 0, nullptr, 0.F, 1.F, ImVec2(300.F, 50.F));
            ImGui::NewLine();

            ImGui::Text("Vertices %d", stats.vertices);
            ImGui::PlotLines("", vertices.data(), 120, 0, nullptr, 0.F, 1.F, ImVec2(300.F, 50.F));
            ImGui::NewLine();

            ImGui::Text("Texture Binds %d", stats.textures);
            ImGui::PlotLines("", textures.data(), 120, 0, nullptr, 0.F, 1.F, ImVec2(300.F, 50.F));
            ImGui::NewLine();

            ImGui::Text("Shader Binds %d", stats.shaders);
            ImGui::PlotLines("", shaders.data(), 120, 0, nullptr, 0.F, 1.F, ImVec2(300.F, 50.F));

            ImGui::End();
        }
    }

} // namespace rosa