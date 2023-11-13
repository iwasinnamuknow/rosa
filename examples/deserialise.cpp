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

// Define the uuid for the image asset we'll use. See assets/assets.lst
const char* yaml_path{ "serialise.yaml" };

auto main() -> int {

    // Grab the GameManager
    auto game_mgr = rosa::GameManager(800, 600);

    rosa::ResourceManager::instance().registerAssetPack("base.pak", "");

    // Create a scene from yaml
    game_mgr.unpackScene("deserialise", yaml_path);

    // Set the scene as active
    game_mgr.changeScene("deserialise");

    // Away we go with our desired window size
    game_mgr.run();

    return 0;
}