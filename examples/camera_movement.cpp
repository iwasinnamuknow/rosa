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


// Rosa objects we'll need
#include <core/GameManager.hpp>
#include <core/components/SpriteComponent.hpp>
#include <core/components/CameraComponent.hpp>
#include <core/components/LuaScriptComponent.hpp>

// Define the uuid for the image asset we'll use. See assets/manifest.yaml
static constexpr auto dds_uuid = rosa::Uuid("f7055f22-6bfa-1a3b-4dbd-b366dd18866d");
static constexpr auto cam_script_uuid = rosa::Uuid("739c0f32-872e-e264-7fa9-57ebaa40c420");

// Create a class to represent our scene
class MyScene : public rosa::Scene {
public:

    // Pass default params to the base class constructor
    explicit MyScene(rosa::RenderWindow* render_window) : rosa::Scene(render_window) {
        // Grab the window size
        auto window_size = getRenderWindow().getSize();

        // Calculate a screen-centered position for the image
        const auto position = glm::vec2(
                (static_cast<float>(window_size.x) / 2.F),
                (static_cast<float>(window_size.y) / 2.F)
        );

        // Create a blank entity. It's not really blank, every entity has a TransformComponent
        // by default.
        auto& entity = createEntity();

        // Add a SpriteComponent to it.
        entity.addComponent<rosa::SpriteComponent>();

        // Set the sprites texture. This is via uuid, not the object we obtained earlier.
        entity.getComponent<rosa::SpriteComponent>().setTexture(dds_uuid);

        // Set the position to screen-center
        entity.getComponent<rosa::TransformComponent>().setPosition(position.x, position.y);

        // Create an entity for the camera
        auto& cam_entity = createEntity();

        // Add a camera component and enable it
        auto& cam_comp = cam_entity.addComponent<rosa::CameraComponent>();
        cam_comp.setEnabled(/*enable=*/true);

        // Add movement script to camera entity
        auto& cam_script_comp = cam_entity.addComponent<rosa::LuaScriptComponent>(this, cam_entity);
        cam_script_comp.setScript(cam_script_uuid);
    }
};

// Entrypoint
auto main() -> int {

    // Grab the GameManager
    auto game_mgr = rosa::GameManager(800, 600, "Camera Movement");

    rosa::ResourceManager::getInstance().registerAssetPack("base.pak", "");

    // Instantiate our scene from the class above and register it
    game_mgr.addScene("camera_movement", std::make_unique<MyScene>(game_mgr.getRenderWindow()));

    // Set the scene as active
    game_mgr.changeScene("camera_movement");

    // Away we go with our desired window size
    game_mgr.run();

    return 0;
}