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

#include <snitch/snitch.hpp>
#include <core/Scene.hpp>
#include <core/Entity.hpp>

TEST_CASE("Allows us to create an entity in a scene", "[scene]") {

    auto scene = rosa::Scene();
    auto& entity = scene.createEntity();

    REQUIRE(scene.getRegistry().storage<entt::entity>().in_use() == 1);
    REQUIRE(entity.getUUID() != rosa::Uuid());
    REQUIRE(entity.hasComponent<rosa::TransformComponent>() == true);
}

TEST_CASE("Allows us to create an entity in a scene with a Uuid", "[scene]") {

    auto uuid = rosa::Uuid::generate();

    auto scene = rosa::Scene();
    auto& entity = scene.createEntity(uuid);

    REQUIRE(scene.getRegistry().storage<entt::entity>().in_use() == 1);
    REQUIRE(entity.getUUID() == uuid);
    REQUIRE(entity.hasComponent<rosa::TransformComponent>() == true);
}

TEST_CASE("Allows us to create an entity in a scene and remove it", "[scene]") {

    auto scene = rosa::Scene();
    auto& entity = scene.createEntity();

    REQUIRE(scene.getRegistry().storage<entt::entity>().in_use() == 1);
    REQUIRE(scene.removeEntity(entity.getUUID()) == true);
    scene.update(0.F);
    REQUIRE(scene.getRegistry().storage<entt::entity>().in_use() == 0);
}

TEST_CASE("Create an entity and retrieve it", "[scene]") {

    auto scene = rosa::Scene();
    auto& original_entity = scene.createEntity();

    auto& retrieved_entity = scene.getEntity(original_entity.getUUID());

    REQUIRE(original_entity == retrieved_entity);
}