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


// Snitch for testing
#include <snitch/snitch.hpp>

// Rosa objects we'll need
#include <core/GameManager.hpp>
#include <core/components/SpriteComponent.hpp>
#include <graphics/ImageComparator.hpp>

// Define the uuid for the image asset we'll use. See assets/assets.lst
static constexpr auto dds_uuid = rosa::Uuid("f7055f22-6bfa-1a3b-4dbd-b366dd18866d");
static constexpr auto filtered_uuid = rosa::Uuid("caa424ce-db6a-ac0e-7874-4734b1534f65");

// Create a class to represent our scene
class FilterScene : public rosa::Scene {
public:

    // Pass default params to the base class constructor
    explicit FilterScene(rosa::RenderWindow* render_window) : rosa::Scene(render_window) {}

    rosa::Uuid target_texture{};

    // Override the onLoad function so we can set up our scene. This will be called
    // any time the GameManager activates the scene.
    auto onLoad() -> void override {

        // Grab the window size
        auto window_size = getRenderWindow().getWindowSize();

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
        entity.getComponent<rosa::SpriteComponent>().setTexture(target_texture);

        // Set the position to screen-center
        entity.getComponent<rosa::TransformComponent>().setPosition(position.x, position.y);

        // Rotate the sprite to break up straight lines
        entity.getComponent<rosa::TransformComponent>().setRotation(-0.125);
    }
};

TEST_CASE("Render a sprite with default NEAREST texture filtering", "[renderer]") {

    // Grab the GameManager
    auto game_mgr = rosa::GameManager(800, 600, "Texture Filtering NEAREST", 1, true);

    rosa::ResourceManager::getInstance().registerAssetPack("references/base.pak", "");

    // Instantiate our scene from the class above and register it
    auto scene = std::make_unique<FilterScene>(game_mgr.getRenderWindow());

    // Set the appropriate texture
    scene->target_texture = dds_uuid;

    game_mgr.addScene("filtering-nearest", std::move(scene));

    // Set the scene as active
    game_mgr.changeScene("filtering-nearest");

    // Advance a few frames
    game_mgr.run(3);

    // Read current framebuffer and compare to the reference image
    game_mgr.getRenderWindow()->getFrameBuffer().copyColorBuffer();
    auto pixels = game_mgr.getRenderWindow()->readFrame();
    std::vector<unsigned char> ref3_pixels = rosa::ImageComparator::readPNG("references/filter_nearest.png");
    REQUIRE(rosa::ImageComparator::compareEqualityBasic(pixels, ref3_pixels) == true);
}

TEST_CASE("Render a sprite with LINEAR texture filtering", "[renderer]") {

// int main(){

    // Grab the GameManager
    auto game_mgr = rosa::GameManager(800, 600, "Texture Filtering LINEAR", 1, true);

    rosa::ResourceManager::getInstance().registerAssetPack("references/base.pak", "");

    // Instantiate our scene from the class above and register it
    auto scene = std::make_unique<FilterScene>(game_mgr.getRenderWindow());

    // Set the appropriate texture
    scene->target_texture = filtered_uuid;

    game_mgr.addScene("filtering-linear", std::move(scene));

    // Set the scene as active
    game_mgr.changeScene("filtering-linear");

    // Advance a few frames
    game_mgr.run(3);

    // Read current framebuffer and compare to the reference image
    game_mgr.getRenderWindow()->getFrameBuffer().copyColorBuffer();
    auto pixels = game_mgr.getRenderWindow()->readFrame();
    std::vector<unsigned char> ref3_pixels = rosa::ImageComparator::readPNG("references/filter_linear.png");
    REQUIRE(rosa::ImageComparator::compareEqualityBasic(pixels, ref3_pixels) == true);
}
