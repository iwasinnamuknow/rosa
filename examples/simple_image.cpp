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
#include <rosa.hpp>

// Define the uuid for the image asset we'll use. See assets/assets.lst
constexpr uuids::uuid dds_uuid = uuids::uuid::from_string(std::string_view("135535e9-6f75-44ba-8d31-236bb9c7f21b")).value();

// Create a class to represent our scene
class MyScene : public rosa::Scene {
    public:

        // Pass default params to the base class constructor
        explicit MyScene(rosa::RenderWindow& render_window) : rosa::Scene(render_window) {}

        // Override the onLoad function so we can set up our scene. This will be called
        // any time the GameManager activates the scene.
        auto onLoad() -> void override {

            // Get our texture via uuid so we can get some details
            auto texture = rosa::ResourceManager::instance().getTexture(dds_uuid);

            // Like the size
            auto texture_size = texture.getSize();

            // Currently the window size is fixed, so silly. Lets get it
            auto window_size = getRenderWindow().getSize();

            // Calculate a screen-centered position for the image
            const auto position = glm::vec2(
                (static_cast<float>(getRenderWindow().getSize().x) / 2.F),
                (static_cast<float>(getRenderWindow().getSize().y) / 2.F)
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

auto main(int argc, char** argv) -> int {

    // We need to pass the path so ResourceManager can find the assets. this is wrong and will be changed.
    auto args = std::span(argv, static_cast<size_t>(argc));
    PHYSFS_init(args[0]);

    // Grab the GameManager
    auto& game_mgr = rosa::GameManager::instance();

    // Instantiate our scene from the class above and register it
    game_mgr.addScene("simple_image", std::make_unique<MyScene>(game_mgr.getRenderWindow()));

    // Set the scene as active
    game_mgr.changeScene("simple_image");

    // Away we go
    game_mgr.run();

    return 0;
}