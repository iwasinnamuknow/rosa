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

#include <core/NativeScriptEntity.hpp>
#include <core/Uuid.hpp>
#include <core/components/TextComponent.hpp>

static constexpr auto font_uuid  = rosa::Uuid("286d945e-7fd9-4444-a95e-3faa3e541536");
static constexpr auto rosa_uuid  = rosa::Uuid("88ce43f7-3ccd-4d0a-867f-13f359a0c658");
static constexpr auto light_uuid = rosa::Uuid("14b87e5e-8a05-4c8f-b850-39ea5b40caf8");

class RosaTalker : public rosa::NativeScriptEntity {
public:
    RosaTalker(rosa::Scene* scene, rosa::Entity* entity);
    ~RosaTalker() override = default;

    enum State {
        Begin        = 0x00,
        Welcome      = 0x01,
        Instructions = 0x02,
        Play         = 0x03,
        Results      = 0x04
    };

    auto onCreate() -> void override;
    auto onUpdate(float delta_time) -> void override;
    auto onInput(const rosa::Event& event) -> void override;

    auto getState() -> State {
        return m_state;
    }

    static rosa::NativeScriptEntity* factoryCreator(rosa::Scene* scene, rosa::Entity* entity) {
        return new RosaTalker(scene, entity);
    }

protected:
    auto getName() const -> std::string override {
        return {"RosaTalker"};
    }

    State                m_state{State::Welcome};
    State                m_previous_state{State::Begin};
    rosa::TextComponent* m_text_comp{nullptr};
    glm::vec2            m_rosa_size;
    rosa::Entity*        m_light_entity{nullptr};

    float       m_typing_timer;
    std::size_t m_typing_char_count;

    std::unordered_map<int, std::string> m_strings{
            {State::Welcome, "Hello, my name is Rosa. Would you like to play a game?"},
            {State::Instructions, "Press any key when the light turns on."},
    };

    std::unique_ptr<std::mt19937_64>      m_generator{nullptr};
    std::uniform_real_distribution<float> m_dist{};

    float m_light_timer;
};