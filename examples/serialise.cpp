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
#include "core/Scene.hpp"
#include "core/components/MusicPlayerComponent.hpp"
#include "core/components/SoundPlayerComponent.hpp"
#include <rosa.hpp>

// Define the uuid for the image asset we'll use. See assets/assets.lst
constexpr uuids::uuid dds_uuid = uuids::uuid::from_string(std::string_view("135535e9-6f75-44ba-8d31-236bb9c7f21b")).value();
constexpr uuids::uuid bell_uuid = uuids::uuid::from_string(std::string_view("dc7d91a9-7ec5-4a5f-b165-373033e5ea56")).value();
constexpr uuids::uuid music_uuid = uuids::uuid::from_string(std::string_view("aaa6b473-f7a9-4a9e-8035-664643178ac4")).value();

// Create a class to represent our scene
class SerialiseScene : public rosa::Scene {
    public:

        // Pass default params to the base class constructor
        explicit SerialiseScene(rosa::RenderWindow& render_window) : rosa::Scene(render_window) {}

        // Override the onLoad function so we can set up our scene. This will be called
        // any time the GameManager activates the scene.
        auto onLoad() -> void override {

            // Get our texture via uuid so we can get some details
            auto texture = rosa::ResourceManager::instance().getAsset<rosa::Texture>(dds_uuid);

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

            auto& lsc = entity.addComponent<rosa::LuaScriptComponent>(this, entity);

            auto& splayer = entity.addComponent<rosa::SoundPlayerComponent>();
            splayer.setAudio(bell_uuid);

            auto& mplayer = entity.addComponent<rosa::MusicPlayerComponent>();
            mplayer.setAudio(music_uuid);

            lsc.setScript(uuids::uuid::from_string("5f20e19c-3749-465b-98ff-d36ead0405ad").value());
        }

        ~SerialiseScene() override {
            auto serialiser = rosa::SceneSerialiser(*this);
            serialiser.serialiseToYaml("serialise.yaml");
        }
};

auto main() -> int {

    // Grab the GameManager
    auto game_mgr = rosa::GameManager(800, 600);

    // Instantiate our scene from the class above and register it
    game_mgr.addScene("serialise", std::make_unique<SerialiseScene>(game_mgr.getRenderWindow()));

    // Set the scene as active
    game_mgr.changeScene("serialise");

    // Away we go with our desired window size
    game_mgr.run();

    return 0;
}