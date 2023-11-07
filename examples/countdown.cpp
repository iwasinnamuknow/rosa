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


// Bring in everything
#include "core/Event.hpp"
#include "core/NativeScriptEntity.hpp"
#include "core/components/SoundPlayerComponent.hpp"
#include "graphics/Sprite.hpp"
#include <bits/chrono.h>
#include <rosa.hpp>
#include <chrono>

// Define the uuid for the image asset we'll use. See assets/assets.lst
constexpr uuids::uuid dds_uuid = uuids::uuid::from_string(std::string_view("a276b044-8a70-4729-b0e1-bf143957aeeb")).value();
constexpr uuids::uuid alarm_uuid = uuids::uuid::from_string(std::string_view("ec1015eb-1d91-4f50-b5ab-4e1696e9def7")).value();

using namespace std::literals;

static int countdown_start{1200};

class CountdownScript : public rosa::NativeScriptEntity {
    public:
        ROSA_CONSTRUCTOR(CountdownScript)

        auto onInput(const rosa::Event& event) -> void {
            switch(event.type) {
                case rosa::EventType::EventResize:
                    auto& sprite = getEntity().getComponent<rosa::SpriteComponent>();

                    glm::vec2 start_pos = sprite.getVertices().front().position;
                    glm::vec2 end_pos = sprite.getVertices().back().position;

                    // Calculate a screen-centered position for the image
                    const auto position = glm::vec2(
                        (static_cast<float>(event.resize.size.x) / 2.F) - (static_cast<float>(end_pos.x - start_pos.x) / 2.F),
                        (static_cast<float>(event.resize.size.y) / 2.F) - (static_cast<float>(end_pos.y - start_pos.y) / 2.F)
                    );
                    getEntity().getComponent<rosa::TransformComponent>().setPosition(position.x, position.y);
                    break;
            }
        }

        void onCreate() override {
            spdlog::info("Test script initialised");
            
            auto& sprite = getEntity().getComponent<rosa::SpriteComponent>();
            auto& texture = sprite.getTexture();
            auto texture_size = texture.getSize();
            
            auto& verts = sprite.getVertices();
            verts.resize(48);
            verts.insert(verts.begin(), {
                {glm::vec2(0,    0)},   // H
                {glm::vec2(63,    0)},
                {glm::vec2(0,     127)},
                {glm::vec2(63,    127)},

                {glm::vec2(64,    0)},  // h
                {glm::vec2(127,   0)},
                {glm::vec2(64,    127)},
                {glm::vec2(127,   127)},
                
                {glm::vec2(128,    0)}, // :
                {glm::vec2(147,    0)},
                {glm::vec2(128,   127)},
                {glm::vec2(147,   127)},

                {glm::vec2(148,   0)}, // M
                {glm::vec2(211,   0)},
                {glm::vec2(148,   127)},
                {glm::vec2(211,   127)},

                {glm::vec2(212,   0)}, // m
                {glm::vec2(275,   0)},
                {glm::vec2(212,   127)},
                {glm::vec2(275,   127)},

                {glm::vec2(276,   0)},  // :
                {glm::vec2(295,   0)},
                {glm::vec2(276,   127)},
                {glm::vec2(295,   127)},

                {glm::vec2(296,   0)},  // S
                {glm::vec2(359,   0)},
                {glm::vec2(296,   127)},
                {glm::vec2(359,   127)},

                {glm::vec2(360,   0)},  // s
                {glm::vec2(423,   0)},
                {glm::vec2(360,   127)},
                {glm::vec2(423,   127)},

                {glm::vec2(424,   0)},  // .
                {glm::vec2(443,   0)},
                {glm::vec2(424,   127)},
                {glm::vec2(443,   127)},

                {glm::vec2(444,   0)},  // mS
                {glm::vec2(507,   0)},
                {glm::vec2(444,   127)},
                {glm::vec2(507,   127)},

                {glm::vec2(508,   0)}, // ms
                {glm::vec2(571,   0)},
                {glm::vec2(508,   127)},
                {glm::vec2(571,   127)},

                {glm::vec2(572,   0)}, // ms
                {glm::vec2(635,   0)},
                {glm::vec2(572,   127)},
                {glm::vec2(635,   127)}
            });

            auto& indices = sprite.getIndices();
            indices.resize(72);
            indices.insert(indices.begin(), {
                0,     1,    2,    // H
                1,     2,    3,

                4,     5,    6,    // h
                5,    6,   7,

                8,   9,   10,   // :
                9,   10,  11,

                12,  13,  14,   // M
                13,  14,  15,

                16,  17,  18,   // m
                17,  18,  19,

                20,  21,  22,   // :
                21,  22,  23,

                24,  25,  26,   // S
                25,  26,  27,

                28,  29,  30,   // s
                29,  30,  31,

                32,  33,  34,   // .
                33,  34,  35,

                36,  37,  38,   // mS
                37,  38,  39,

                40,  41,  42,   // ms
                41,  42,  43,

                44,  45,  46,   // ms
                45,  46,  47
            });

            m_start_time = std::chrono::steady_clock::now() + std::chrono::seconds(countdown_start);

            // Preset texture co-ords for static items
            verts[8].texture_coords =  glm::vec2(279 / texture_size.x, 128 / texture_size.y);
            verts[9].texture_coords =  glm::vec2(296 / texture_size.x, 128 / texture_size.y);
            verts[10].texture_coords = glm::vec2(279 / texture_size.x, 255 / texture_size.y);
            verts[11].texture_coords = glm::vec2(296 / texture_size.x, 255 / texture_size.y);

            verts[20].texture_coords = glm::vec2(279 / texture_size.x, 128 / texture_size.y);
            verts[21].texture_coords = glm::vec2(296 / texture_size.x, 128 / texture_size.y);
            verts[22].texture_coords = glm::vec2(279 / texture_size.x, 255 / texture_size.y);
            verts[23].texture_coords = glm::vec2(296 / texture_size.x, 255 / texture_size.y);

            verts[32].texture_coords = glm::vec2((279 + 63) / texture_size.x, 128 / texture_size.y);
            verts[33].texture_coords = glm::vec2((296 + 63) / texture_size.x, 128 / texture_size.y);
            verts[34].texture_coords = glm::vec2((279 + 63) / texture_size.x, 255 / texture_size.y);
            verts[35].texture_coords = glm::vec2((296 + 63) / texture_size.x, 255 / texture_size.y);

            verts.resize(48);
            indices.resize(72);
        }

        void onUpdate(float delta_time) override {
            auto& player = getEntity().getComponent<rosa::SoundPlayerComponent>();

            if (m_finished) {
                
                //spdlog::debug("{}", player.

            } else {

                auto& sprite = getEntity().getComponent<rosa::SpriteComponent>();
                auto& verts = sprite.getVertices();
                auto& texture = sprite.getTexture();
                auto texture_size = texture.getSize();

                m_current_time = std::chrono::steady_clock::now();
                auto duration = m_start_time - m_current_time;

                if (std::chrono::duration_cast<std::chrono::nanoseconds>(duration).count() <= 0) {
                    m_finished = true;
                    duration = std::chrono::seconds::zero();
                    player.play();
                }

                // Hours
                int hours = std::floor(std::chrono::duration_cast<std::chrono::hours>(duration).count());
                duration -= (1h * hours);
                int h_10 = (hours / 10);
                verts[0].texture_coords = glm::vec2(((h_10 % 6) * 64) / texture_size.x,      ( h_10 > 5 ? 128 : 0) / texture_size.y);
                verts[1].texture_coords = glm::vec2(((h_10 % 6) * 64 + 64) / texture_size.x, ( h_10 > 5 ? 128 : 0) / texture_size.y);
                verts[2].texture_coords = glm::vec2(((h_10 % 6) * 64) / texture_size.x,      ((h_10 > 5 ? 128 : 0) + 127) / texture_size.y);
                verts[3].texture_coords = glm::vec2(((h_10 % 6) * 64 + 64) / texture_size.x, ((h_10 > 5 ? 128 : 0) + 127) / texture_size.y);
                hours -= (h_10 * 10);
                verts[4].texture_coords = glm::vec2(((hours % 6) * 64) / texture_size.x,      ( hours > 5 ? 128 : 0) / texture_size.y);
                verts[5].texture_coords = glm::vec2(((hours % 6) * 64 + 64) / texture_size.x, ( hours > 5 ? 128 : 0) / texture_size.y);
                verts[6].texture_coords = glm::vec2(((hours % 6) * 64) / texture_size.x,      ((hours > 5 ? 128 : 0) + 127) / texture_size.y);
                verts[7].texture_coords = glm::vec2(((hours % 6) * 64 + 64) / texture_size.x, ((hours > 5 ? 128 : 0) + 127) / texture_size.y);

                // Minutes
                int minutes = std::floor(std::chrono::duration_cast<std::chrono::minutes>(duration).count());
                duration -= (1min * minutes);
                int m_10 = (minutes / 10);
                verts[12].texture_coords = glm::vec2(((m_10 % 6) * 64) / texture_size.x,      ( m_10 > 5 ? 128 : 0) / texture_size.y);
                verts[13].texture_coords = glm::vec2(((m_10 % 6) * 64 + 64) / texture_size.x, ( m_10 > 5 ? 128 : 0) / texture_size.y);
                verts[14].texture_coords = glm::vec2(((m_10 % 6) * 64) / texture_size.x,      ((m_10 > 5 ? 128 : 0) + 127) / texture_size.y);
                verts[15].texture_coords = glm::vec2(((m_10 % 6) * 64 + 64) / texture_size.x, ((m_10 > 5 ? 128 : 0) + 127) / texture_size.y);
                minutes -= (m_10 * 10);
                verts[16].texture_coords = glm::vec2(((minutes % 6) * 64) / texture_size.x,      ( minutes > 5 ? 128 : 0) / texture_size.y);
                verts[17].texture_coords = glm::vec2(((minutes % 6) * 64 + 64) / texture_size.x, ( minutes > 5 ? 128 : 0) / texture_size.y);
                verts[18].texture_coords = glm::vec2(((minutes % 6) * 64) / texture_size.x,      ((minutes > 5 ? 128 : 0) + 127) / texture_size.y);
                verts[19].texture_coords = glm::vec2(((minutes % 6) * 64 + 64) / texture_size.x, ((minutes > 5 ? 128 : 0) + 127) / texture_size.y);

                // Seconds
                int seconds = std::floor(std::chrono::duration_cast<std::chrono::seconds>(duration).count());
                duration -= (1s * seconds);
                int s_10 = (seconds / 10);
                verts[24].texture_coords = glm::vec2(((s_10 % 6) * 64) / texture_size.x,      ( s_10 > 5 ? 128 : 0) / texture_size.y);
                verts[25].texture_coords = glm::vec2(((s_10 % 6) * 64 + 64) / texture_size.x, ( s_10 > 5 ? 128 : 0) / texture_size.y);
                verts[26].texture_coords = glm::vec2(((s_10 % 6) * 64) / texture_size.x,      ((s_10 > 5 ? 128 : 0) + 127) / texture_size.y);
                verts[27].texture_coords = glm::vec2(((s_10 % 6) * 64 + 64) / texture_size.x, ((s_10 > 5 ? 128 : 0) + 127) / texture_size.y);
                seconds -= (s_10 * 10);
                verts[28].texture_coords = glm::vec2(((seconds % 6) * 64) / texture_size.x,      ( seconds > 5 ? 128 : 0) / texture_size.y);
                verts[29].texture_coords = glm::vec2(((seconds % 6) * 64 + 64) / texture_size.x, ( seconds > 5 ? 128 : 0) / texture_size.y);
                verts[30].texture_coords = glm::vec2(((seconds % 6) * 64) / texture_size.x,      ((seconds > 5 ? 128 : 0) + 127) / texture_size.y);
                verts[31].texture_coords = glm::vec2(((seconds % 6) * 64 + 64) / texture_size.x, ((seconds > 5 ? 128 : 0) + 127) / texture_size.y);

                // Milliseconds
                int millis = std::floor(std::chrono::duration_cast<std::chrono::milliseconds>(duration).count());
                int ms_100 = (millis / 100);
                verts[36].texture_coords = glm::vec2(((ms_100 % 6) * 64) / texture_size.x,      ( ms_100 > 5 ? 128 : 0) / texture_size.y);
                verts[37].texture_coords = glm::vec2(((ms_100 % 6) * 64 + 64) / texture_size.x, ( ms_100 > 5 ? 128 : 0) / texture_size.y);
                verts[38].texture_coords = glm::vec2(((ms_100 % 6) * 64) / texture_size.x,      ((ms_100 > 5 ? 128 : 0) + 127) / texture_size.y);
                verts[39].texture_coords = glm::vec2(((ms_100 % 6) * 64 + 64) / texture_size.x, ((ms_100 > 5 ? 128 : 0) + 127) / texture_size.y);
                millis -= (ms_100 * 100);
                int ms_10 = (millis / 10);
                verts[40].texture_coords = glm::vec2(((ms_10 % 6) * 64) / texture_size.x,      ( ms_10 > 5 ? 128 : 0) / texture_size.y);
                verts[41].texture_coords = glm::vec2(((ms_10 % 6) * 64 + 64) / texture_size.x, ( ms_10 > 5 ? 128 : 0) / texture_size.y);
                verts[42].texture_coords = glm::vec2(((ms_10 % 6) * 64) / texture_size.x,      ((ms_10 > 5 ? 128 : 0) + 127) / texture_size.y);
                verts[43].texture_coords = glm::vec2(((ms_10 % 6) * 64 + 64) / texture_size.x, ((ms_10 > 5 ? 128 : 0) + 127) / texture_size.y);
                millis -= (ms_10 * 10);
                verts[44].texture_coords = glm::vec2(((millis % 6) * 64) / texture_size.x,      ( millis > 5 ? 128 : 0) / texture_size.y);
                verts[45].texture_coords = glm::vec2(((millis % 6) * 64 + 64) / texture_size.x, ( millis > 5 ? 128 : 0) / texture_size.y);
                verts[46].texture_coords = glm::vec2(((millis % 6) * 64) / texture_size.x,      ((millis > 5 ? 128 : 0) + 127) / texture_size.y);
                verts[47].texture_coords = glm::vec2(((millis % 6) * 64 + 64) / texture_size.x, ((millis > 5 ? 128 : 0) + 127) / texture_size.y);
            }
        }

        void onDestroy() override {
            spdlog::info("Dieing!");
        }

    private:
        std::chrono::time_point<std::chrono::steady_clock> m_start_time;
        std::chrono::time_point<std::chrono::steady_clock> m_current_time;

        bool m_finished{false};
};

// Create a class to represent our scene
class CountdownScene : public rosa::Scene {
    public:

        // Pass default params to the base class constructor
        explicit CountdownScene(rosa::RenderWindow& render_window) : rosa::Scene(render_window) {}

        // Override the onLoad function so we can set up our scene. This will be called
        // any time the GameManager activates the scene.
        auto onLoad() -> void override {

            // Get our texture via uuid so we can get some details
            auto texture = rosa::ResourceManager::instance().getTexture(dds_uuid);

            // Like the size
            auto texture_size = texture.getSize();

            // Grab the window size
            auto window_size = getRenderWindow().getSize();

            // Create a blank entity. It's not really blank, every entity has a TransformComponent
            // by default.
            auto& entity = createEntity();

            getRegistry().emplace<rosa::NativeScriptComponent>(entity).bind<CountdownScript>();

            // Add a SpriteComponent to it.
            rosa::Sprite& sprite = entity.addComponent<rosa::SpriteComponent>();

            glm::vec2 start_pos = sprite.getVertices().front().position;
            glm::vec2 end_pos = sprite.getVertices().back().position;

            // Calculate a screen-centered position for the image
            const auto position = glm::vec2(
                (static_cast<float>(window_size.x) / 2.F) - (static_cast<float>(end_pos.x - start_pos.x) / 2.F),
                (static_cast<float>(window_size.y) / 2.F) - (static_cast<float>(end_pos.y - start_pos.y) / 2.F)
            );

            // Set the sprites texture. This is via uuid, not the object we obtained earlier.
            entity.getComponent<rosa::SpriteComponent>().setTexture(dds_uuid);

            // Set the position to screen-center
            entity.getComponent<rosa::TransformComponent>().setPosition(position.x, position.y);

            auto& player = entity.addComponent<rosa::SoundPlayerComponent>();
            player.setAudio(alarm_uuid);
            player.setLooping(true);
        }
};

auto main(int argc, char* argv[]) -> int {

    if (argc > 1) {
        countdown_start = std::stoi(std::string(argv[1]));
    }

    // Grab the GameManager
    auto game_mgr = rosa::GameManager(800, 600);

    // Instantiate our scene from the class above and register it
    game_mgr.addScene("countdown", std::make_unique<CountdownScene>(game_mgr.getRenderWindow()));

    // Set the scene as active
    game_mgr.changeScene("countdown");

    // Away we go with our desired window size
    game_mgr.run();

    return 0;
}