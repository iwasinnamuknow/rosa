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
#include "NSCCountdown.hpp"
#include <core/GameManager.hpp>
#include <core/components/SoundPlayerComponent.hpp>
#include <iostream>

// Default countdown duration
static int countdown_start{1200};

// Create a class to represent our scene
class CountdownScene : public rosa::Scene {
public:

    // Pass default params to the base class constructor
    explicit CountdownScene(rosa::RenderWindow* render_window) : rosa::Scene(render_window) {
        auto& parent = createEntity();

        for (int i = 0; i < 12; i++) {
            auto& entity = createEntity();
            entity.setParent(parent.getUuid());
            entity.addComponent<rosa::SpriteComponent>();
        }

        parent.addComponent<rosa::SoundPlayerComponent>();
        auto& nsc = parent.addComponent<rosa::NativeScriptComponent>();
        nsc.bind<NSCCountdown>();
        nsc.instantiate_function(this, &parent);
        nsc.on_create_function(nsc.instance);
        dynamic_cast<NSCCountdown*>(nsc.instance)->countdown_time = countdown_start;
        dynamic_cast<NSCCountdown*>(nsc.instance)->start();
    }
};

// Entrypoint
auto main(int argc, char* argv[]) -> int {

    // Parse arguments for countdown duration
    if (argc > 1) {
        try {
            countdown_start = std::stoi(std::string(argv[1]));
        } catch (std::exception& e) {
            std::cout << "Error parsing time from argument, " << e.what() << "\n";
        }
    }

    // Grab the GameManager
    auto game_mgr = rosa::GameManager(800, 600, "Countdown Timer");

    rosa::ResourceManager::getInstance().registerAssetPack("base.pak", "");

    // Instantiate our scene from the class above and register it
    game_mgr.addScene("countdown", std::make_unique<CountdownScene>(game_mgr.getRenderWindow()));

    // Set the scene as active
    game_mgr.changeScene("countdown");

    // Away we go with our desired window size
    game_mgr.run();

    return 0;
}