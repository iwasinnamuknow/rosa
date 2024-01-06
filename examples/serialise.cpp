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
#include <core/GameManager.hpp>
#include <core/components/MusicPlayerComponent.hpp>
#include <core/components/SoundPlayerComponent.hpp>
#include <core/components/SpriteComponent.hpp>
#include <core/components/LuaScriptComponent.hpp>
#include <core/SceneSerialiser.hpp>
#include <core/components/NativeScriptComponent.hpp>
#include <core/NativeScriptEntity.hpp>

// Define the uuid for the image asset we'll use. See assets/assets.lst
static const auto dds_uuid = rosa::Uuid("f7055f226bfa1a3b4dbdb366dd18866d");
static const auto bell_uuid = rosa::Uuid("60f200640c132e9435cd404c011c00a2");
static const auto music_uuid = rosa::Uuid("a3c890ea0f97d3ed32de55ba88c8dc63");
static const auto script_uuid = rosa::Uuid("9046e8fecb017adf1029c79e71961173");

class NSCTest : public rosa::NativeScriptEntity {
    public:
        ROSA_CONSTRUCTOR(NSCTest)
        ~NSCTest() override = default;

        void onCreate() override {
            spdlog::info("NativeScript Test script initialised");
        }

        void onLoad() override {
            spdlog::info("NativeScript Test script loaded");
        }

        void onUpdate(float delta_time) override {
            m_test_int++;
            m_test_string = "test_string" + std::to_string(m_test_int);
        }

        void onDestroy() override {
            spdlog::info("NativeScript Dieing!");
        }

        static rosa::NativeScriptEntity* factoryCreator(rosa::Scene* scene, rosa::Entity* entity) {
            return new NSCTest(*scene, *entity);
        }

    protected:
        auto serialise() -> YAML::Node override {

            YAML::Node res;

            res["test_int"] = m_test_int;
            res["test_string"] = m_test_string;

            return res;
        }

        auto deserialise(YAML::Node node) -> void override {
            if (node.Type() == YAML::NodeType::Map) {
                if (node["test_int"]) {
                    m_test_int = node["test_int"].as<int>();
                }

                if (node["test_string"]) {
                    m_test_string = node["test_string"].as<std::string>();
                }
            }

            onLoad();
        }

        auto getName() -> std::string override {
            return {"NSCTest"};
        }

        int m_test_int{1234};
        std::string m_test_string{"testing1234"};
};

// Create a class to represent our scene
class SerialiseScene : public rosa::Scene {
    public:

        // Pass default params to the base class constructor
        explicit SerialiseScene(rosa::RenderWindow* render_window) : rosa::Scene(render_window) {}

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

            lsc.setScript(script_uuid);

            // Create a NativeScriptComponent to test serialisation of that too
            getRegistry().emplace<rosa::NativeScriptComponent>(entity).bind<NSCTest>();
        }

        ~SerialiseScene() override {
            auto serialiser = rosa::SceneSerialiser(*this);
            serialiser.registerNSC("NSCTest", &NSCTest::factoryCreator);
            serialiser.serialiseToYaml("serialise.yaml");
        }
};

auto main() -> int {

    // Grab the GameManager
    auto game_mgr = rosa::GameManager(800, 600);

    rosa::ResourceManager::instance().registerAssetPack("base.pak", "");

    // Instantiate our scene from the class above and register it
    game_mgr.addScene("serialise", std::make_unique<SerialiseScene>(game_mgr.getRenderWindow()));

    // Set the scene as active
    game_mgr.changeScene("serialise");

    // Away we go with our desired window size
    game_mgr.run();

    return 0;
}