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

#include "MyScene.hpp"
#include "core/components/LuaScriptComponent.hpp"

MyScene::MyScene(sf::RenderWindow& render_window) : rosa::Scene(render_window) {
    auto new_entity = createEntity();
    new_entity.addComponent<rosa::SpriteComponent>();

    //getRegistry().emplace<rosa::NativeScriptComponent>(new_entity).bind<TestScript>();

    auto& lsc = new_entity.addComponent<rosa::LuaScriptComponent>();
    lsc.setScript("test.lua");
}

// auto MyScene::update(float delta_time) -> void {

// }

// auto MyScene::render() -> void {

// }