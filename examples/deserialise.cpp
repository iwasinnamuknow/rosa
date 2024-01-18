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
#include "core/GameManager.hpp"
#include <core/SceneSerialiser.hpp>

// Define the uuid for the image asset we'll use. See assets/manifest.yaml
const char* yaml_path{ "serialise.yaml" };

// NativeScript test class
#include "NSCTest.hpp"

// Create a class to represent our scene
class DeserialiseScene : public rosa::Scene {
public:

    // Pass default params to the base class constructor
    explicit DeserialiseScene(rosa::RenderWindow* render_window) : rosa::Scene(render_window) {}

    // Override the onLoad function so we can set up our scene. This will be called
    // any time the GameManager activates the scene.
    auto onLoad() -> void override {
    }
};

auto main() -> int {

    // Grab the GameManager
    auto game_mgr = rosa::GameManager(800, 600);

    rosa::ResourceManager::getInstance().registerAssetPack("base.pak", "");

    // Create the scene based on our example above
    auto scene = std::make_unique<DeserialiseScene>(game_mgr.getRenderWindow());

    auto serialiser = std::make_unique<rosa::SceneSerialiser>(*scene);

    // We need to register our NativeScript class factory creation function with the serialiser
    serialiser->registerNSC("NSCTest", &NSCTest::factoryCreator);

    // Deserialise our saved file into the scene
    serialiser->deserialiseFromYaml(yaml_path);

    // Add the scene to the game manager
    game_mgr.addScene("deserialise_scene", std::move(scene));

    // Set the scene as active
    game_mgr.changeScene("deserialise_scene");

    // Away we go with our desired window size
    game_mgr.run();

    return 0;
}