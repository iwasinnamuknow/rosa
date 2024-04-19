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

#include "RosaTalker.hpp"

RosaTalker::RosaTalker(rosa::Scene* scene, rosa::Entity* entity)
    : NativeScriptEntity(scene, entity),
      m_rosa_size(rosa::ResourceManager::getInstance().getAsset<rosa::Texture>(rosa_uuid).getSize()),
      m_generator(std::make_unique<std::mt19937_64>(std::random_device()())),
      m_dist(std::uniform_real_distribution(-0.1F, 0.3F)) {
}

auto RosaTalker::onCreate() -> void {

    // Sprite setup
    auto& sprite_comp = getEntity().addComponent<rosa::SpriteComponent>();
    sprite_comp.setTexture(rosa_uuid);
    sprite_comp.setScreenSpace(true);

    // Text setup
    m_text_comp = &getEntity().addComponent<rosa::TextComponent>();
    m_text_comp->setFont(font_uuid);
    m_text_comp->setColour(rosa::Colour(1.F, 0.F, 0.F, 1.F));
    m_text_comp->setOffset(glm::vec2(120.F, 0.F));
    m_text_comp->setScreenSpace(true);

    // Entity position
    const auto window_size = getScene().getRenderWindow().getWindowSize();
    auto       position    = glm::vec2(
            (static_cast<float>((m_rosa_size.x / 2))),
            (static_cast<float>(window_size.y) - (m_rosa_size.y / 2)));
    getEntity().getComponent<rosa::TransformComponent>().setPosition(position);
}

auto RosaTalker::onUpdate(float delta_time) -> void {

    // handle text states
    if (m_previous_state != m_state) {
        m_typing_char_count = 0;
        m_typing_timer      = 0.F;
        m_previous_state    = m_state;

        if (m_state == State::Results) {
            if (m_light_timer > 0.F) {
                m_strings[Results] = std::format("Too early, calm down.");
            } else {
                m_strings[Results] = std::format("You missed it by {} seconds", m_light_timer * -1.F);
            }
        }
    } else if (m_typing_char_count < m_strings[m_state].size()) {
        if (m_typing_timer > (0.1F + m_dist(*m_generator))) {
            m_typing_char_count++;
            m_text_comp->setText(m_strings[m_state].substr(0, m_typing_char_count));
            m_typing_timer = 0.F;
        } else {
            m_typing_timer += delta_time;
        }
    }

    if (m_state == State::Play) {
        if (m_light_timer <= 0.F) {
            m_light_entity->getComponent<rosa::SpriteComponent>().setTextureRect(rosa::Rect(glm::vec2(128, 0), glm::vec2(128, 128)));
        }

        m_light_timer -= delta_time;
    }
}

auto RosaTalker::onInput(const rosa::Event& event) -> void {
    if (event.type == rosa::EventType::EventResize) {
        const auto window_size = getScene().getRenderWindow().getWindowSize();
        auto       position    = glm::vec2(
                (static_cast<float>((m_rosa_size.x / 2))),
                (static_cast<float>(window_size.y) - (m_rosa_size.y / 2)));
        getEntity().getComponent<rosa::TransformComponent>().setPosition(position);

        if (m_light_entity != nullptr) {
            position = glm::vec2(
                    (static_cast<float>(window_size.x / 2)),
                    (static_cast<float>(window_size.y / 2)));
            m_light_entity->getComponent<rosa::TransformComponent>().setPosition(position);
        }

    } else if (event.type == rosa::EventType::EventKeyboard) {
        if (event.keyboard.type == rosa::KeyboardEventType::KeyReleased) {
            switch (m_state) {
                case Welcome:
                    if (m_typing_char_count < m_strings[Welcome].size()) {
                        m_typing_char_count = m_strings[Welcome].size();
                        m_text_comp->setText(m_strings[Welcome]);
                    } else {
                        m_state = Instructions;
                    }
                    break;
                case Instructions:
                    if (m_typing_char_count < m_strings[Instructions].size()) {
                        m_typing_char_count = m_strings[Instructions].size();
                        m_text_comp->setText(m_strings[Instructions]);
                    } else {
                        m_state = Play;
                        std::uniform_real_distribution<float> dist{std::uniform_real_distribution(2.F, 5.F)};
                        m_light_timer = dist(*m_generator);
                        m_text_comp->setText("");
                        getScene().deferCall([this]() -> void {
                            m_light_entity     = &getScene().createEntity();
                            auto& light_sprite = m_light_entity->addComponent<rosa::SpriteComponent>();
                            light_sprite.setTexture(light_uuid);
                            light_sprite.setTextureRect(rosa::Rect(glm::vec2(0, 0), glm::vec2(128, 128)));
                            const auto window_size = getScene().getRenderWindow().getWindowSize();
                            const auto position    = glm::vec2(
                                    (static_cast<float>(window_size.x / 2)),
                                    (static_cast<float>(window_size.y / 2)));
                            m_light_entity->getComponent<rosa::TransformComponent>().setPosition(position);
                        });
                    }
                    break;
                case Play:
                    m_state = Results;
                    m_light_entity->die();
                    break;
                case Results:
                    if (m_typing_char_count < m_strings[Results].size()) {
                        m_typing_char_count = m_strings[Results].size();
                        m_text_comp->setText(m_strings[Results]);
                    } else {
                        m_state = Instructions;
                    }
                    break;
            }
        }
    }
}