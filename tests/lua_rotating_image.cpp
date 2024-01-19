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
#include <core/components/LuaScriptComponent.hpp>
#include <graphics/ImageComparator.hpp>

// Define the uuid for the image asset we'll use. See assets/assets.lst
static constexpr auto dds_uuid = rosa::Uuid("f7055f22-6bfa-1a3b-4dbd-b366dd18866d");
static constexpr auto script_uuid = rosa::Uuid("9046e8fe-cb01-7adf-1029-c79e71961173");

// Create a class to represent our scene
class LuaRotatingImage : public rosa::Scene {
public:

    // Pass default params to the base class constructor
    explicit LuaRotatingImage(rosa::RenderWindow* render_window) : rosa::Scene(render_window) {}

    // Override the onLoad function so we can set up our scene. This will be called
    // any time the GameManager activates the scene.
    auto onLoad() -> void override {

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

        // Creat the lua script component and load our script
        auto& script_comp = entity.addComponent<rosa::LuaScriptComponent>(this, entity);
        script_comp.setScript(script_uuid);
    }
};

TEST_CASE("Rotate an image via lua script", "[lua]") {

    // Grab the GameManager
    auto game_mgr = rosa::GameManager(800, 600, "Lua Rotate Image", 0, true);

    rosa::ResourceManager::getInstance().registerAssetPack("references/base.pak", "");

    // Instantiate our scene from the class above and register it
    game_mgr.addScene("simple_image", std::make_unique<LuaRotatingImage>(game_mgr.getRenderWindow()));

    // Set the scene as active
    game_mgr.changeScene("simple_image");

    // Advance some frames
    game_mgr.run(3);

    // Read current framebuffer and compare to the reference image
    game_mgr.getRenderWindow()->getFrameBuffer().copyColorBuffer();
    auto pixels = game_mgr.getRenderWindow()->readFrame();
    std::vector<unsigned char> ref3_pixels = rosa::ImageComparator::readPNG("references/lua_rotating_image-3.png");
    REQUIRE(rosa::ImageComparator::compareEqualityBasic(pixels, ref3_pixels) == true);

    // Advance some more frames
    game_mgr.run(3);

    // Read current framebuffer and compare to the reference image
    game_mgr.getRenderWindow()->getFrameBuffer().copyColorBuffer();
    pixels = game_mgr.getRenderWindow()->readFrame();
    std::vector<unsigned char> ref6_pixels = rosa::ImageComparator::readPNG("references/lua_rotating_image-6.png");
    REQUIRE(rosa::ImageComparator::compareEqualityBasic(pixels, ref6_pixels) == true);
}