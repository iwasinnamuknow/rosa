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

#pragma once

#include <core/NativeScriptEntity.hpp>
#include <core/GameManager.hpp>
#include <spdlog/spdlog.h>

class TestScript : public rosa::NativeScriptEntity {
    public:
        ROSA_CONSTRUCTOR(TestScript)

        void onCreate() override {
            spdlog::info("Test script initialised");
            auto& texture = rosa::ResourceManager::instance().getTexture("rosa.png");
            getEntity().getComponent<rosa::SpriteComponent>().sprite.setTexture(texture);
            const sf::Vector2f position = sf::Vector2f(
                (static_cast<float>(getScene().getRenderWindow().getSize().x) / 2.F) - (static_cast<float>(texture.getSize().x) / 2.F),
                (static_cast<float>(getScene().getRenderWindow().getSize().y) / 2.F) - (static_cast<float>(texture.getSize().y) / 2.F)
            );
            getEntity().getComponent<rosa::TransformComponent>().position = position;
        }

        void onUpdate(float delta_time) override {
            //std::cout << "DeltaTime: " << delta_time << std::endl;
            spdlog::debug("Deltatime: {}", delta_time);
            // spdlog::debug("attempting to kill");
            // die();
        }

        void onDestroy() override {
            spdlog::info("Dieing!");
        }
};