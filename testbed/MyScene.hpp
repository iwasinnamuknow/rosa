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

#include <core/Scene.hpp>
#include <core/components/SpriteComponent.hpp>
#include <core/components/NativeScriptComponent.hpp>
#include <core/components/LuaScriptComponent.hpp>
#include "TestScript.hpp"

class MyScene : public rosa::Scene {
    public:
        explicit MyScene(sf::RenderWindow& render_window);
        ~MyScene() override = default;

        // auto update(float delta_time) -> void override;
        // auto render() -> void override;
};