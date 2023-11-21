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
#include <snitch/snitch.hpp>
#include <core/GameManager.hpp>
#include <core/components/SpriteComponent.hpp>
#include <graphics/ImageComparator.hpp>

// Define the uuid for the image asset we'll use. See assets/assets.lst
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

            // Set the position to screen-center
            entity.getComponent<rosa::TransformComponent>().setPosition(position.x, position.y);
        }
};

TEST_CASE("Displays a simple 2d image on a quad", "[gl]") {

    // Grab the GameManager
    auto game_mgr = rosa::GameManager(800, 600, "Display Image", true);

    rosa::ResourceManager::instance().registerAssetPack("references/base.pak", "");

    // Instantiate our scene from the class above and register it
    game_mgr.addScene("simple_image", std::make_unique<MyScene>(game_mgr.getRenderWindow()));

    // Set the scene as active
    game_mgr.changeScene("simple_image");

    // Away we go with our desired window size
    game_mgr.run(3);

    auto size = game_mgr.getRenderWindow()->getSize();
    
    game_mgr.getRenderWindow()->getFrameBuffer().copyColorBuffer();
    std::vector<unsigned char> pixels = game_mgr.getRenderWindow()->readFrame();
    //rosa::ImageComparator::writePNG("test.png", pixels, size.x, size.y);

    std::vector<unsigned char> ref_pixels = rosa::ImageComparator::readPNG("references/display_image.png");
    
    REQUIRE(rosa::ImageComparator::compareEqualityBasic(pixels, ref_pixels) == true);
}