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
#include <core/GameManager.hpp>
#include <core/components/MusicPlayerComponent.hpp>
#include <core/components/SoundPlayerComponent.hpp>
#include <core/components/SpriteComponent.hpp>
#include <core/components/LuaScriptComponent.hpp>
#include <core/SceneSerialiser.hpp>

// Define the uuid for the image asset we'll use. See assets/manifest.yaml
static constexpr auto dds_uuid    = rosa::Uuid("f7055f22-6bfa-1a3b-4dbd-b366dd18866d");
static constexpr auto bell_uuid   = rosa::Uuid("60f20064-0c13-2e94-35cd-404c011c00a2");
static constexpr auto music_uuid  = rosa::Uuid("a3c890ea-0f97-d3ed-32de-55ba88c8dc63");
static constexpr auto script_uuid = rosa::Uuid("9046e8fe-cb01-7adf-1029-c79e71961173");

// NativeScript test class
#include "NSCTest.hpp"

// Create a class to represent our scene
class SerialiseScene : public rosa::Scene {
    public:

        // Pass default params to the base class constructor
        explicit SerialiseScene(rosa::RenderWindow* render_window) : rosa::Scene(render_window) {}

        // Override the onLoad function so we can set up our scene. This will be called
        // any time the GameManager activates the scene.
        auto onLoad() -> void override {

            // Get our texture via uuid so we can get some details
            auto texture = rosa::ResourceManager::getInstance().getAsset<rosa::Texture>(dds_uuid);

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

            // Create lua script
            auto& lsc = entity.addComponent<rosa::LuaScriptComponent>(this, entity);

            // Create sound player
            auto& splayer = entity.addComponent<rosa::SoundPlayerComponent>();
            splayer.setAudio(bell_uuid);

            // Create music player
            auto& mplayer = entity.addComponent<rosa::MusicPlayerComponent>();
            mplayer.setAudio(music_uuid);

            // Initialise the lua script
            lsc.setScript(script_uuid);

            // Create a NativeScriptComponent to test serialisation of that too
            getRegistry().emplace<rosa::NativeScriptComponent>(entity).bind<NSCTest>();
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