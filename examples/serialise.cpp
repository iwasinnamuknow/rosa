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
#include <core/GameManager.hpp>
#include <core/SceneSerialiser.hpp>
#include <core/components/MusicPlayerComponent.hpp>
#include <core/components/NativeScriptComponent.hpp>
#include <core/components/SoundPlayerComponent.hpp>
#include <core/components/SpriteComponent.hpp>

// Define the uuid for the assets we'll use. See assets/manifest.yaml
static constexpr auto dds_uuid = rosa::Uuid("f7055f22-6bfa-1a3b-4dbd-b366dd18866d");
static constexpr auto bell_uuid = rosa::Uuid("60f20064-0c13-2e94-35cd-404c011c00a2");
static constexpr auto music_uuid = rosa::Uuid("a3c890ea-0f97-d3ed-32de-55ba88c8dc63");

// NativeScript test class
#include "NSCTest.hpp"

// Create a class to represent our scene
class SerialiseScene : public rosa::Scene {
public:

    // Pass default params to the base class constructor
    explicit SerialiseScene(rosa::RenderWindow* render_window)
        : rosa::Scene(render_window) {

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
        auto& sprite = entity.addComponent<rosa::SpriteComponent>();

        // Set the sprites texture. This is via uuid, not the object we obtained earlier.
        sprite.setTexture(dds_uuid);

        // Set the position to screen-center
        entity.getComponent<rosa::TransformComponent>().setPosition(position.x, position.y);

        // Create sound player
        auto& splayer = entity.addComponent<rosa::SoundPlayerComponent>();
        splayer.setAudio(bell_uuid);

        // Create music player
        auto& mplayer = entity.addComponent<rosa::MusicPlayerComponent>();
        mplayer.setAudio(music_uuid);

        // Create a NativeScriptComponent to test serialisation of that too
        entity.addComponent<rosa::NativeScriptComponent>().bind<NSCTest>();
    }

    ~SerialiseScene() override {
        // On destruction we will create a serialiser class
        auto serialiser = rosa::SceneSerialiser(*this);
        // Register our NativeScript class factory creation function
        serialiser.registerNSC("NSCTest", &NSCTest::factoryCreator);
        // Serialise our scene to file
        serialiser.serialiseToYaml("serialise.yaml");
    }
};

auto main() -> int {

    // Grab the GameManager
    auto game_mgr = rosa::GameManager(800, 600);

    rosa::ResourceManager::getInstance().registerAssetPack("base.pak", "");

    // Instantiate our scene from the class above and register it
    game_mgr.addScene("serialise", std::make_unique<SerialiseScene>(game_mgr.getRenderWindow()));

    // Set the scene as active
    game_mgr.changeScene("serialise");

    // Away we go with our desired window size
    game_mgr.run();

    return 0;
}