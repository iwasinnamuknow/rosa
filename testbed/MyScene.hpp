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
//#include "TestScript.hpp"
#include <core/SceneSerialiser.hpp>

constexpr uuids::uuid rosa_uuid = uuids::uuid::from_string(std::string_view("b79dc541-f2f5-49b2-9af6-22693f3ee4da")).value();
constexpr uuids::uuid lua_uuid = uuids::uuid::from_string(std::string_view("5f20e19c-3749-465b-98ff-d36ead0405ad")).value();

class MyScene : public rosa::Scene {
    public:
        explicit MyScene(sf::RenderWindow& render_window);
        ~MyScene() override {
            auto ser = rosa::SceneSerialiser(*this);
            ser.serialiseToYaml("test.yaml");
        }

        //auto update(float delta_time) -> void override;
        // auto render() -> void override;
};