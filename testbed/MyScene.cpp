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
#include "core/Scene.hpp"
#include "core/components/LuaScriptComponent.hpp"
#include "debug/Profiler.hpp"
#include <cstddef>
#include <cstdint>
#include <unordered_map>

MyScene::MyScene(rosa::RenderWindow& render_window) : rosa::Scene(render_window) {
    
    
}

auto MyScene::onLoad() -> void {
    auto texture = rosa::ResourceManager::instance().getTexture(dds_uuid);
    auto texture_size = texture.getSize();
    auto window_size = getRenderWindow().getSize();
    const auto position = glm::vec2(
        (static_cast<float>(getRenderWindow().getSize().x) / 2.F) - (static_cast<float>(texture.getSize().x) / 2.F),
        (static_cast<float>(getRenderWindow().getSize().y) / 2.F) - (static_cast<float>(texture.getSize().y) / 2.F)
    );

    auto& parent_entity = createEntity();
    parent_entity.addComponent<rosa::SpriteComponent>();
    parent_entity.getComponent<rosa::SpriteComponent>().setTexture(dds_uuid);
    parent_entity.getComponent<rosa::TransformComponent>().setPosition(position.x, position.y);
    // TODO
    //parent_entity.getComponent<rosa::TransformComponent>().setPosition(position.x, position.y);

    // auto& child_entity = createEntity();
    // child_entity.addComponent<rosa::SpriteComponent>();
    // child_entity.getComponent<rosa::SpriteComponent>().setTexture(rosa_uuid);
    // child_entity.setParent(parent_entity.getId());
    // child_entity.getComponent<rosa::TransformComponent>().setPosition(-50, -50);

    //getRegistry().emplace<rosa::NativeScriptComponent>(parent_entity).bind<TestScript>();
    auto& lsc = parent_entity.addComponent<rosa::LuaScriptComponent>(*this, parent_entity);
    lsc.setScript(lua_uuid);
}

// auto MyScene::render() -> void {

// }