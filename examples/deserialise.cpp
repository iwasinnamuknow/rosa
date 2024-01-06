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
#include <core/NativeScriptEntity.hpp>
#include <core/components/NativeScriptComponent.hpp>
#include <core/SceneSerialiser.hpp>

// Define the uuid for the image asset we'll use. See assets/assets.lst
const char* yaml_path{ "serialise.yaml" };

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

    rosa::ResourceManager::instance().registerAssetPack("base.pak", "");

    auto scene = std::make_unique<DeserialiseScene>(game_mgr.getRenderWindow());

    auto serialiser = std::make_unique<rosa::SceneSerialiser>(*scene);
    serialiser->registerNSC("NSCTest", &NSCTest::factoryCreator);
    serialiser->deserialiseFromYaml(yaml_path);

    // Create a scene from yaml
    game_mgr.addScene("deserialise_scene", std::move(scene));

    // Set the scene as active
    game_mgr.changeScene("deserialise_scene");

    // Away we go with our desired window size
    game_mgr.run();

    return 0;
}