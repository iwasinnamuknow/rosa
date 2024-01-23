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
#include "core/GameManager.hpp"
#include <core/SceneSerialiser.hpp>

// NativeScript test class
#include "NSCTest.hpp"

// Create a class to represent our scene
class DeserialiseScene : public rosa::Scene {
public:

    // Pass default params to the base class constructor
    explicit DeserialiseScene(rosa::RenderWindow* render_window) : rosa::Scene(render_window) {}
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
    serialiser->deserialiseFromYaml("serialise.yaml");

    // Add the scene to the game manager
    game_mgr.addScene("deserialise_scene", std::move(scene));

    // Set the scene as active
    game_mgr.changeScene("deserialise_scene");

    // Away we go with our desired window size
    game_mgr.run();

    return 0;
}