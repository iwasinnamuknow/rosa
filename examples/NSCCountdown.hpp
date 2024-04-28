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
#include <core/GameManager.hpp>
#include <core/NativeScriptEntity.hpp>
#include <core/components/NativeScriptComponent.hpp>
#include <core/components/SoundPlayerComponent.hpp>
#include <core/input/Keyboard.hpp>
#include <format>

constexpr auto digits_uuid = rosa::Uuid("409b5f84-4060-0a2e-9227-04056ac17ffb");
constexpr auto alarm_uuid  = rosa::Uuid("47185140-8a1b-9d46-3606-402ed23bcf83");

// This is our NativeScript test class
class NSCCountdown : public rosa::NativeScriptEntity {
public:
    ROSA_CONSTRUCTOR(NSCCountdown)
    ~NSCCountdown() override = default;

    int countdown_time{0};

    void onCreate() override {
        int         counter{0};
        const auto& children = getEntity().getChildren();

        for (const auto& child: children) {
            auto& real_child  = getScene().getEntity(child);
            auto& transform   = real_child.getComponent<rosa::TransformComponent>();
            auto& sprite      = real_child.getComponent<rosa::SpriteComponent>();
            char_order[child] = counter;
            sprite.setTexture(digits_uuid);

            if (counter == 2 || counter == 5) {// colons
                sprite.setTextureRect(characters.at(":"));
                transform.setPosition(total_width - 20, 0);
                total_width += 20;
            } else if (counter == 8) {// period
                sprite.setTextureRect(characters.at("."));
                transform.setPosition(total_width - 20, 0);
                total_width += 20;
            } else {// digits
                sprite.setTextureRect(characters.at("0"));
                transform.setPosition(total_width, 0);
                total_width += 64;
            }

            counter++;
        }

        auto       start_pos   = glm::vec2(0, 0);
        auto       end_pos     = glm::vec2(total_width, 128);
        const auto window_size = getScene().getRenderWindow().getWindowSize();

        auto position = glm::vec2(
                ((window_size.x / 2.0F) - ((end_pos.x - start_pos.x) / 2.0F)),
                ((window_size.y / 2.0F) - ((end_pos.y - start_pos.y) / 2.0F)));

        getEntity().getComponent<rosa::TransformComponent>().setPosition(position);
        auto& player = getEntity().getComponent<rosa::SoundPlayerComponent>();
        player.setAudio(alarm_uuid);
        player.setLooping(true);
    }

    void onUpdate(float delta_time) override {
        //spdlog::debug("Formatted time: {}", getTimeFormatted());

        std::chrono::time_point<std::chrono::steady_clock> time_now    = std::chrono::steady_clock::now();
        auto                                               target_time = m_start_time + std::chrono::seconds(countdown_time);
        auto                                               duration    = std::chrono::duration_cast<std::chrono::milliseconds>(target_time - time_now);

        auto time_string = getTimeFormatted();
        if (m_running) {
            if (duration.count() <= 0) {
                m_running = false;
                getEntity().getComponent<rosa::SoundPlayerComponent>().play();
            }

            const auto& children = getEntity().getChildren();
            for (const auto& child_uuid: children) {
                auto& child_entity = getScene().getEntity(child_uuid);
                auto& sprite       = child_entity.getComponent<rosa::SpriteComponent>();
                auto  index        = char_order[child_uuid];

                if (index != 2 && index != 5 && index != 8) {
                    auto character = time_string.substr(index, 1);
                    auto rect      = characters.at(character);
                    sprite.setTextureRect(rect);
                }
            }
        }
    }

    void onInput(const rosa::Event& event) override {
        if (event.type == rosa::EventType::EventResize) {
            const auto start_pos   = glm::vec2(0.F, 0.F);
            const auto end_pos     = glm::vec2(total_width, 0.F);
            const auto window_size = getScene().getRenderWindow().getWindowSize();
            const auto position    = glm::vec2(
                    ((window_size.x / 2.0) - ((end_pos.x - start_pos.x) / 2.0)),
                    ((window_size.y / 2.0) - ((end_pos.y - start_pos.y) / 2.0)));
            getEntity().getComponent<rosa::TransformComponent>().setPosition(position);
        }
    }

    void start() {
        m_start_time = std::chrono::steady_clock::now();
        m_running    = true;
    }

    std::string getTimeFormatted() {
        std::chrono::time_point<std::chrono::steady_clock> time_now     = std::chrono::steady_clock::now();
        auto                                               target_time  = m_start_time + std::chrono::seconds(countdown_time);
        auto                                               duration     = target_time - time_now;
        auto                                               formatted    = std::chrono::hh_mm_ss(duration);
        auto                                               full_seconds = std::chrono::floor<std::chrono::seconds>(duration);
        auto                                               millis       = std::chrono::duration_cast<std::chrono::milliseconds>(duration - full_seconds);
        return std::format("{:%H}:{:%M}:{:%S}.{}", duration, duration, duration, millis.count()).substr(0, 12);
    }

    // We need this for deserialisation so we can re-create the class
    static rosa::NativeScriptEntity* factoryCreator(rosa::Scene* scene, rosa::Entity* entity) {
        return new NSCCountdown(scene, entity);
    }

protected:
    // Provide serialisation, yea you have to manually deal with yaml
    auto serialise() const -> YAML::Node override {
        YAML::Node res;
        return res;
    }

    // Deserialisation, you also have to deal with yaml here
    auto deserialise(YAML::Node node) -> void override {
        onLoad();
    }

    auto getName() const -> std::string override {
        return {"NSCCountdown"};
    }

    const int                                         char_height{128};
    const int                                         char_width{64};
    int                                               total_width{0};
    std::unordered_map<rosa::Uuid, int>               char_order{};
    const std::unordered_map<std::string, rosa::Rect> characters{
            {"0", {{char_width * 0, char_height * 0}, {char_width, char_height}}},
            {"1", {{char_width * 1, char_height * 0}, {char_width, char_height}}},
            {"2", {{char_width * 2, char_height * 0}, {char_width, char_height}}},
            {"3", {{char_width * 3, char_height * 0}, {char_width, char_height}}},
            {"4", {{char_width * 4, char_height * 0}, {char_width, char_height}}},
            {"5", {{char_width * 5, char_height * 0}, {char_width, char_height}}},
            {"6", {{char_width * 0, char_height * 1}, {char_width, char_height}}},
            {"7", {{char_width * 1, char_height * 1}, {char_width, char_height}}},
            {"8", {{char_width * 2, char_height * 1}, {char_width, char_height}}},
            {"9", {{char_width * 3, char_height * 1}, {char_width, char_height}}},
            {":", {{char_width * 4 + 22, char_height * 1}, {20, char_height}}},
            {".", {{char_width * 5 + 22, char_height * 1}, {20, char_height}}}};

    std::chrono::time_point<std::chrono::steady_clock> m_start_time;
    bool                                               m_running{false};
};