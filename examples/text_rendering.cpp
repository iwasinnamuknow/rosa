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


// Bring in everything
#include <core/GameManager.hpp>
#include <core/components/TextComponent.hpp>
#include <core/components/CameraComponent.hpp>
#include <core/components/LuaScriptComponent.hpp>
#include <core/Entity.hpp>

// Define the uuid for the image asset we'll use. See assets/assets.lst
static const auto font_uuid = rosa::Uuid("286d945e-7fd9-4444-a95e-3faa3e541536");
static const auto cam_script_uuid = rosa::Uuid("739c0f32872ee2647fa957ebaa40c420");
static const auto dds_uuid = rosa::Uuid("f7055f226bfa1a3b4dbdb366dd18866d");

// Create a class to represent our scene
class MyScene : public rosa::Scene {
    public:

        // Pass default params to the base class constructor
        explicit MyScene(rosa::RenderWindow* render_window) : rosa::Scene(render_window) {}

        // Override the onLoad function so we can set up our scene. This will be called
        // any time the GameManager activates the scene.
        auto onLoad() -> void override {

            // Get our texture via uuid so we can get some details
            auto font = rosa::ResourceManager::instance().getAsset<rosa::BitmapFont>(font_uuid);

            // Get our texture via uuid so we can get some details
            auto texture = rosa::ResourceManager::instance().getAsset<rosa::Texture>(dds_uuid);

            // Like the size
            auto texture_size = texture.getSize();

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
            entity.getComponent<rosa::SpriteComponent>().setBatched(true);

            auto& text_comp = entity.addComponent<rosa::TextComponent>();
            text_comp.setFont(font_uuid);
            text_comp.setText("Test String");
            text_comp.setColour(rosa::Colour(1.F, 0.F, 0.F, 1.F));
            text_comp.setScreenSpace(false);

            // Set the position
            entity.getComponent<rosa::TransformComponent>().setPosition(position.x, position.y);

            auto& cam_entity = createEntity();
            auto& cam_comp = cam_entity.addComponent<rosa::CameraComponent>();
            cam_comp.setEnabled(true);

            auto& cam_script_comp = cam_entity.addComponent<rosa::LuaScriptComponent>(this, cam_entity);
            cam_script_comp.setScript(cam_script_uuid);
        }
};

auto main() -> int {

    // Grab the GameManager
    auto game_mgr = rosa::GameManager(800, 600);

    rosa::ResourceManager::instance().registerAssetPack("base.pak", "");

    // Instantiate our scene from the class above and register it
    game_mgr.addScene("simple_image", std::make_unique<MyScene>(game_mgr.getRenderWindow()));

    // Set the scene as active
    game_mgr.changeScene("simple_image");

    // Away we go with our desired window size
    game_mgr.run();

    return 0;
}