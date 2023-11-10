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
#include "core/components/NativeScriptComponent.hpp"
#include "core/components/SoundPlayerComponent.hpp"
#include "core/components/SpriteComponent.hpp"
#include "core/components/TransformComponent.hpp"
#include "graphics/Sprite.hpp"
#include <rosa.hpp>
#include <chrono>
#include <graphics/Rect.hpp>
#include <unordered_map>
#include <fmt/format.h>

using namespace std::literals;

// Define the uuid for the image asset we'll use. See assets/assets.lst
static const auto dds_uuid = rosa::Uuid("409b5f8440600a2e922704056ac17ffb");
static const auto alarm_uuid = rosa::Uuid("471851408a1b9d463606402ed23bcf83");

static int countdown_start{1200};
constexpr int char_height{128};
constexpr int char_width{64};

std::unordered_map<char, rosa::Rect> characters{
    {'0', {{char_width * 0, char_height * 0     }, {char_width, char_height}}},
    {'1', {{char_width * 1, char_height * 0     }, {char_width, char_height}}},
    {'2', {{char_width * 2, char_height * 0     }, {char_width, char_height}}},
    {'3', {{char_width * 3, char_height * 0     }, {char_width, char_height}}},
    {'4', {{char_width * 4, char_height * 0     }, {char_width, char_height}}},
    {'5', {{char_width * 5, char_height * 0     }, {char_width, char_height}}},
    {'6', {{char_width * 0, char_height * 1 }, {char_width, char_height}}},
    {'7', {{char_width * 1, char_height * 1 }, {char_width, char_height}}},
    {'8', {{char_width * 2, char_height * 1 }, {char_width, char_height}}},
    {'9', {{char_width * 3, char_height * 1 }, {char_width, char_height}}},
    {':', {{char_width * 4 + 22, char_height * 1 }, {20, char_height}}},
    {'.', {{char_width * 5 + 22, char_height * 1 }, {20, char_height}}},
};

template <class Rep, std::intmax_t num, std::intmax_t denom>
auto chronoBurst(std::chrono::duration<Rep, std::ratio<num, denom>> d)
{
    const auto hrs = duration_cast<std::chrono::hours>(d);
    const auto mins = duration_cast<std::chrono::minutes>(d - hrs);
    const auto secs = duration_cast<std::chrono::seconds>(d - hrs - mins);
    const auto ms = duration_cast<std::chrono::milliseconds>(d - hrs - mins - secs);

    return std::make_tuple(hrs, mins, secs, ms);
}

class CountdownScript : public rosa::NativeScriptEntity {
    public:
        ROSA_CONSTRUCTOR(CountdownScript)

        auto onInput(const rosa::Event& event) -> void {
            switch(event.type) {
                case rosa::EventType::EventResize:
                    auto& parent = getEntity();

                    glm::vec2 start_pos = {0, 0};
                    glm::vec2 end_pos = {total_width, 128};

                    // Calculate a screen-centered position for the image
                    const auto position = glm::vec2(
                        (static_cast<float>(event.resize.size.x) / 2.F) - (static_cast<float>(end_pos.x - start_pos.x) / 2.F),
                        (static_cast<float>(event.resize.size.y) / 2.F) - (static_cast<float>(end_pos.y - start_pos.y) / 2.F)
                    );
                    parent.getComponent<rosa::TransformComponent>().setPosition(position.x, position.y);
                    break;
            }
        }

        void onCreate() override {
            spdlog::info("Test script initialised");
            
            // auto& sprite = getEntity().getComponent<rosa::SpriteComponent>();
            // auto& texture = sprite.getTexture();
            // auto texture_size = texture.getSize();

            auto& parent_entity = getEntity();
            
            int counter{0};

            for (auto child : parent_entity.getChildren()) {

                auto& child_entity = getScene().getEntity(child);
                auto& child_sprite = child_entity.getComponent<rosa::SpriteComponent>();
                auto& child_transform = child_entity.getComponent<rosa::TransformComponent>();
                m_character_order.insert({child_entity.getId(), counter});

                child_transform.setPosition(total_width, 0);

                // Handle colon separators
                if (counter == 2 || counter == 5) {
                    auto character = characters[':'];
                    child_sprite.setTextureRect(characters[':']);
                    total_width += 20;
                
                // Handle period separator
                } else if (counter == 8) {
                    child_sprite.setTextureRect(characters['.']);
                    total_width += 20;

                // Set zero for digits initially
                } else {
                    child_sprite.setTextureRect(characters['0']);
                    total_width += 64;
                }

                counter++;
            }

            glm::vec2 start_pos = {0, 0};
            glm::vec2 end_pos = {total_width, 128};
            auto size = getScene().getRenderWindow().getSize();

            // Calculate a screen-centered position for the image
            const auto position = glm::vec2(
                (static_cast<float>(size.x) / 2.F) - (static_cast<float>(end_pos.x - start_pos.x) / 2.F),
                (static_cast<float>(size.y) / 2.F) - (static_cast<float>(end_pos.y - start_pos.y) / 2.F)
            );
            parent_entity.getComponent<rosa::TransformComponent>().setPosition(position.x, position.y);

            m_start_time = std::chrono::steady_clock::now() + std::chrono::seconds(countdown_start);
        }

        void onUpdate(float delta_time) override {
            auto& player = getEntity().getComponent<rosa::SoundPlayerComponent>();

            if (!m_finished) {

                m_current_time = std::chrono::steady_clock::now();
                auto duration = m_start_time - m_current_time;
                if (std::chrono::duration_cast<std::chrono::nanoseconds>(duration).count() <= 0) {
                    m_finished = true;
                    duration = std::chrono::seconds::zero();
                    player.play();
                }
                const auto [hours, minutes, seconds, milliseconds] = chronoBurst(duration);
                auto time_str = fmt::format("{:02}:{:02}:{:02}.{:03}", hours.count(), minutes.count(), seconds.count(), milliseconds.count());
                
                auto& children = getEntity().getChildren();

                for (auto& child : children) {

                    auto& child_entity = getScene().getEntity(child);
                    auto& child_sprite = child_entity.getComponent<rosa::SpriteComponent>();
                    auto index = m_character_order.at(child_entity.getId());

                    // Ignore separators
                    if (index == 2 || index == 5 || index == 8) {
                        continue;

                    // Handle digits
                    } else {
                        auto character = time_str.c_str()[index];
                        child_sprite.setTextureRect(characters[character]);
                    }
                }
            }
        }

        void onDestroy() override {
            spdlog::info("Dieing!");
        }

    private:
        std::chrono::time_point<std::chrono::steady_clock> m_start_time;
        std::chrono::time_point<std::chrono::steady_clock> m_current_time;

        bool m_finished{false};
        std::unordered_map<entt::entity, int> m_character_order;
        int total_width{0};
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
            //auto texture = rosa::ResourceManager::instance().getAsset<rosa::Texture>(dds_uuid);

            // Like the size
            //auto texture_size = texture.getSize();

            // Grab the window size
            //auto window_size = getRenderWindow().getSize();

            auto& parent = createEntity();
            //getRegistry().emplace<rosa::NativeScriptComponent>(entity).bind<CountdownScript>();
            parent.addComponent<rosa::NativeScriptComponent>().bind<CountdownScript>();
            auto& player = parent.addComponent<rosa::SoundPlayerComponent>();
            player.setAudio(alarm_uuid);
            player.setLooping(true);

            for (int i = 0; i < 12; i++) {
                auto& entity = createEntity();
                entity.setParent(parent.getId());
                auto& sprite = entity.addComponent<rosa::SpriteComponent>();
                sprite.setTexture(dds_uuid);
            }

            // Set the sprites texture. This is via uuid, not the object we obtained earlier.
            

            // Set the position to screen-center
            //entity.getComponent<rosa::TransformComponent>().setPosition(position.x, position.y);
        }
};

auto main(int argc, char* argv[]) -> int {

    if (argc > 1) {
        try {
            countdown_start = std::stoi(std::string(argv[1]));
        } catch (std::exception e) {
            std::cout << "Error parsing time from argument, " << e.what() << "\n";
        }
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