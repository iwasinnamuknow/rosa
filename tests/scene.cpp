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

#include <core/Entity.hpp>
#include <core/Scene.hpp>
#include <graphics/Renderer.hpp>
#include <snitch/snitch.hpp>

TEST_CASE("Allows us to create an entity in a scene", "[scene]") {

    auto scene = rosa::Scene();
    auto& entity = scene.createEntity();

    REQUIRE(scene.getRegistry().count() == 1);
    REQUIRE(entity.getUuid() != rosa::Uuid());
    REQUIRE(entity.hasComponent<rosa::TransformComponent>() == true);

    rosa::Renderer::shutdown();
}

TEST_CASE("Allows us to create an entity in a scene with a Uuid", "[scene]") {

    auto uuid = rosa::Uuid::generate();

    auto scene = rosa::Scene();
    auto& entity = scene.createEntity(uuid);

    REQUIRE(scene.getRegistry().count() == 1);
    REQUIRE(entity.getUuid() == uuid);
    REQUIRE(entity.hasComponent<rosa::TransformComponent>() == true);

    rosa::Renderer::shutdown();
}

TEST_CASE("Allows us to create an entity in a scene and remove it", "[scene]") {

    auto scene = rosa::Scene();
    auto& entity = scene.createEntity();

    REQUIRE(scene.getRegistry().count() == 1);
    REQUIRE(scene.removeEntity(entity.getUuid()) == true);
    scene.update(0.F);
    REQUIRE(scene.getRegistry().count() == 0);

    rosa::Renderer::shutdown();
}

TEST_CASE("Create an entity and retrieve it", "[scene]") {

    auto scene = rosa::Scene();
    auto& original_entity = scene.createEntity();

    auto& retrieved_entity = scene.getEntity(original_entity.getUuid());

    REQUIRE(original_entity == retrieved_entity);

    rosa::Renderer::shutdown();
}