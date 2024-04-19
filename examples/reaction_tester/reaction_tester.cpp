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


// Bring in everything
#include "RosaTalker.hpp"
#include <core/Entity.hpp>
#include <core/GameManager.hpp>
#include <core/components/NativeScriptComponent.hpp>

// Create a class to represent our scene
class GameScene : public rosa::Scene {
public:
    // Pass default params to the base class constructor
    explicit GameScene(rosa::RenderWindow* render_window)
        : rosa::Scene(render_window) {}

    // Override the onLoad function so we can set up our scene. This will be called
    // any time the GameManager activates the scene.
    auto onLoad() -> void override {

        // Rosa head/text
        auto& rosa_entity = createEntity();
        rosa_entity.addComponent<rosa::NativeScriptComponent>().bind<RosaTalker>();
    }
};

auto main() -> int {

    // Grab the GameManager
    auto game_mgr = rosa::GameManager(800, 600, "Reaction Test", 8);

    rosa::ResourceManager::getInstance().registerAssetPack("base.pak", "");

    // Instantiate our scene from the class above and register it
    game_mgr.addScene("game_scene", std::make_unique<GameScene>(game_mgr.getRenderWindow()));

    // Set the scene as active
    game_mgr.changeScene("game_scene");

    // Away we go with our desired window size
    game_mgr.run();

    return 0;
}