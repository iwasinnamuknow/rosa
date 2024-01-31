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


// Snitch for testing
#include <filesystem>
#include <snitch/snitch.hpp>
#include <yaml-cpp/yaml.h>

// Rosa objects we'll need
#include <core/GameManager.hpp>
#include <core/SceneSerialiser.hpp>
#include <core/SerialiserTypes.hpp>
#include <core/components/CameraComponent.hpp>
#include <core/components/LuaScriptComponent.hpp>
#include <core/components/MusicPlayerComponent.hpp>
#include <core/components/SoundPlayerComponent.hpp>
#include <core/components/SpriteComponent.hpp>

static constexpr auto serialise_file = std::string("serialise.yaml");

// Define the uuid for the assets we'll use. See assets/manifest.yaml
static constexpr auto dds_uuid    = rosa::Uuid("f7055f22-6bfa-1a3b-4dbd-b366dd18866d");
static constexpr auto bell_uuid   = rosa::Uuid("60f20064-0c13-2e94-35cd-404c011c00a2");
static constexpr auto music_uuid  = rosa::Uuid("a3c890ea-0f97-d3ed-32de-55ba88c8dc63");
static constexpr auto script_uuid = rosa::Uuid("2404de33-3a8f-4dd3-97d9-5fe2bb579780");

namespace {
    bool camera_entity_found{false};
    bool regular_entity_found{false};
}// namespace

// NativeScript test class
#include "NSCTest.hpp"

// Create a class to represent our scene
class SerialiseScene : public rosa::Scene {
public:
    // Pass default params to the base class constructor
    explicit SerialiseScene(rosa::RenderWindow* render_window)
        : rosa::Scene(render_window) {}

    // Override the onLoad function so we can set up our scene. This will be called
    // any time the GameManager activates the scene.
    auto onLoad() -> void override {

        // Grab the window size
        auto window_size = getRenderWindow().getWindowSize();

        // Calculate a screen-centered position for the image
        const auto position = glm::vec2(
                (static_cast<float>(window_size.x) / 2.F),
                (static_cast<float>(window_size.y) / 2.F));

        // Create a blank entity. It's not really blank, every entity has a TransformComponent
        // by default.
        auto& entity = createEntity();

        // Add a SpriteComponent to it.
        auto& sprite = entity.addComponent<rosa::SpriteComponent>();

        // Set the sprites texture. This is via uuid, not the object we obtained earlier.
        sprite.setTexture(dds_uuid);

        // Set the position to screen-center
        entity.getComponent<rosa::TransformComponent>().setPosition(position.x, position.y);

        // Create lua script
        auto& script_comp = entity.addComponent<rosa::LuaScriptComponent>();

        // Create sound player
        auto& splayer = entity.addComponent<rosa::SoundPlayerComponent>();
        splayer.setAudio(bell_uuid);

        // Create music player
        auto& mplayer = entity.addComponent<rosa::MusicPlayerComponent>();
        mplayer.setAudio(music_uuid);

        script_comp.setScript(entity.getUUID(), this, script_uuid);

        // Create a NativeScriptComponent to test serialisation of that too
        getRegistry().emplace<rosa::NativeScriptComponent>(entity).bind<NSCTest>();

        // Create an entity for the camera
        auto& cam_entity = createEntity();

        // Add a camera component and enable it
        auto& cam_comp = cam_entity.addComponent<rosa::CameraComponent>();
        cam_comp.setEnabled(/*enable=*/true);
    }

    ~SerialiseScene() override {
        // On destruction we will create a serialiser class
        auto serialiser = rosa::SceneSerialiser(*this);
        // Register our NativeScript class factory creation function
        serialiser.registerNSC("NSCTest", &NSCTest::factoryCreator);
        // Serialise our scene to file
        serialiser.serialiseToYaml(serialise_file);
    }
};

auto roughly_equals(float lhs, float rhs) -> bool {
    const float relative_difference_factor = 0.0001;// 0.01%
    const auto  greater_magnitude          = std::max(std::abs(lhs), std::abs(rhs));
    return std::abs(lhs - rhs) <= relative_difference_factor * greater_magnitude;
}

auto check_transform(const YAML::Node& node, glm::vec3 pos_ref, glm::vec3 scale_ref, float rot_ref) -> void {
    // Position
    const auto& position = node["position"];
    REQUIRE(position.Type() == YAML::NodeType::Sequence);
    auto position_value = position.as<glm::vec3>();
    REQUIRE(position_value == pos_ref);

    // Scale
    const auto& scale = node["scale"];
    REQUIRE(scale.Type() == YAML::NodeType::Sequence);
    auto scale_value = scale.as<glm::vec3>();
    REQUIRE(scale_value == scale_ref);

    // Rotation
    const auto& rotation = node["rotation"];
    REQUIRE(rotation.Type() == YAML::NodeType::Scalar);
    auto rotation_value = rotation.as<float>();
    REQUIRE(roughly_equals(rot_ref, rotation_value));
}

auto check_camera_entity(const YAML::Node& components) -> void {
    bool found_transform{false};

    for (const auto& component: components) {
        const auto& type = component["type"];
        REQUIRE(type.Type() == YAML::NodeType::Scalar);

        if (type.as<std::string>() == "transform") {
            check_transform(component, {0.F, 0.F, 0.F}, {1.F, 1.F, 1.F}, 0.F);
            found_transform = true;
        } else {
            REQUIRE(type.as<std::string>() == "camera");
            const auto& enabled = component["enabled"];
            REQUIRE(enabled.Type() == YAML::NodeType::Scalar);
            auto enabled_value = enabled.as<bool>();
            REQUIRE(enabled_value);
        }
    }

    camera_entity_found = true;
    REQUIRE(found_transform);
}

auto check_sprite(const YAML::Node& component) -> void {
    const auto& texture_uuid = component["texture"];
    REQUIRE(texture_uuid.Type() == YAML::NodeType::Scalar);
    auto texture_uuid_value = texture_uuid.as<rosa::Uuid>();
    REQUIRE(texture_uuid_value == dds_uuid);

    const auto& colour = component["colour"];
    REQUIRE(colour.Type() == YAML::NodeType::Sequence);
    auto colour_value = colour.as<rosa::Colour>();
    REQUIRE(colour_value == rosa::Colour(0.46F, 1.F, 1.F, 1.F));
}

auto check_sound(const YAML::Node& component, rosa::Uuid source) -> void {
    const auto& source_uuid = component["source"];
    REQUIRE(source_uuid.Type() == YAML::NodeType::Scalar);
    auto source_uuid_value = source_uuid.as<rosa::Uuid>();
    REQUIRE(source_uuid_value == source);

    const auto& position = component["position"];
    REQUIRE(position.Type() == YAML::NodeType::Scalar);
    auto position_value = position.as<float>();
    REQUIRE(position_value > 0.F && position_value < 1.F);

    const auto& volume = component["volume"];
    REQUIRE(volume.Type() == YAML::NodeType::Scalar);
    auto volume_value = volume.as<float>();
    REQUIRE(roughly_equals(volume_value, 1.F));

    const auto& default_volume = component["default_volume"];
    REQUIRE(default_volume.Type() == YAML::NodeType::Scalar);
    auto default_volume_value = default_volume.as<float>();
    REQUIRE(roughly_equals(default_volume_value, 1.F));

    const auto& paused = component["paused"];
    REQUIRE(paused.Type() == YAML::NodeType::Scalar);
    auto paused_value = paused.as<bool>();
    REQUIRE(paused_value == false);

    const auto& playing = component["playing"];
    REQUIRE(playing.Type() == YAML::NodeType::Scalar);
    auto playing_value = playing.as<bool>();
    REQUIRE(playing_value);
}

auto check_lua(const YAML::Node& component) -> void {
    const auto& script = component["script"];
    REQUIRE(script.Type() == YAML::NodeType::Scalar);
    auto script_uuid_value = script.as<rosa::Uuid>();
    REQUIRE(script_uuid_value == script_uuid);

    const auto& data = component["data"];
    REQUIRE(data.Type() == YAML::NodeType::Map);

    const auto& rotation = data["rotation"];
    REQUIRE(rotation.Type() == YAML::NodeType::Scalar);
    REQUIRE(rotation.Tag() == "!float");
    auto rotation_value = rotation.as<float>();
    REQUIRE(roughly_equals(rotation_value, 2.F));

    const auto& red = data["red"];
    REQUIRE(red.Type() == YAML::NodeType::Scalar);
    REQUIRE(red.Tag() == "!float");
    auto red_value = red.as<float>();
    REQUIRE(roughly_equals(red_value, 0.51F));
}

auto check_native_script(const YAML::Node& component) -> void {
    const auto& script = component["script"];
    REQUIRE(script.Type() == YAML::NodeType::Scalar);
    auto script_value = script.as<std::string>();
    REQUIRE(script_value == "NSCTest");

    const auto& data = component["data"];
    REQUIRE(data.Type() == YAML::NodeType::Map);

    const auto& test_int = data["test_int"];
    REQUIRE(test_int.Type() == YAML::NodeType::Scalar);
    auto test_int_value = test_int.as<int>();
    REQUIRE(test_int_value == 1244);

    const auto& test_string = data["test_string"];
    REQUIRE(test_string.Type() == YAML::NodeType::Scalar);
    auto test_string_value = test_string.as<std::string>();
    REQUIRE(test_string_value == "test_string1244");
}

auto check_entity(const YAML::Node& components) -> void {
    REQUIRE(components.size() == 6);
    bool found_transform{false};
    bool found_sprite{false};
    bool found_sound{false};
    bool found_music{false};
    bool found_lua{false};
    bool found_nsc{false};

    for (const auto& component: components) {
        const auto& type = component["type"];
        REQUIRE(type.Type() == YAML::NodeType::Scalar);

        auto type_value = type.as<std::string>();

        if (type_value == "transform") {
            check_transform(component, {400.F, 300.F, 1.F}, {1.F, 1.F, 1.F}, 2.F);
            found_transform = true;

        } else if (type_value == "sprite") {
            check_sprite(component);
            found_sprite = true;

        } else if (type_value == "sound") {
            check_sound(component, bell_uuid);
            found_sound = true;

        } else if (type_value == "music") {
            check_sound(component, music_uuid);
            found_music = true;

        } else if (type_value == "lua_script") {
            check_lua(component);
            found_lua = true;

        } else if (type_value == "native_script") {
            check_native_script(component);
            found_nsc = true;
        }
    }

    regular_entity_found = true;
    REQUIRE(found_transform);
    REQUIRE(found_sprite);
    REQUIRE(found_sound);
    REQUIRE(found_music);
    REQUIRE(found_lua);
    REQUIRE(found_nsc);
}

auto check_entities(const YAML::Node& entities) -> void {
    for (const auto& entity: entities) {
        REQUIRE(entity.Type() == YAML::NodeType::Map);

        // Check components
        auto components = entity["components"];
        REQUIRE(components.Type() == YAML::NodeType::Sequence);

        // 2 components must be the camera entity
        if (components.size() == 2) {
            check_camera_entity(components);

            // "regular" entity
        } else {
            check_entity(components);
        }
    }

    REQUIRE(camera_entity_found);
    REQUIRE(regular_entity_found);
}

TEST_CASE("Serialise a scene", "[serialiser]") {

    // If a yaml file exists, remove it
    if (std::filesystem::exists(serialise_file)) {
        std::filesystem::remove(serialise_file);
    }

    {
        // Grab the GameManager
        auto game_mgr = rosa::GameManager(800, 600, "Texture Filtering NEAREST", 1, true);

        rosa::ResourceManager::getInstance().registerAssetPack("references/base.pak", "");

        // Instantiate our scene from the class above and register it
        auto scene = std::make_unique<SerialiseScene>(game_mgr.getRenderWindow());

        game_mgr.addScene("serialise", std::move(scene));

        // Set the scene as active
        game_mgr.changeScene("serialise");

        // Advance a few frames
        game_mgr.run(10);
    }

    // Check it wrote out a file
    REQUIRE(std::filesystem::exists(serialise_file));

    // Load the written file and attempt to parse
    YAML::Node yaml_data = YAML::LoadFile(serialise_file);

    // Check the version number
    auto version = yaml_data["version"];
    REQUIRE(version.Type() == YAML::NodeType::Scalar);
    REQUIRE(version.as<int>() == rosa::serialiser_version);

    // Check the scene
    auto scene = yaml_data["scene"];
    REQUIRE(scene.Type() == YAML::NodeType::Map);

    // Check for entity collection
    auto entities = scene["entities"];
    REQUIRE(entities.Type() == YAML::NodeType::Sequence);
    REQUIRE(entities.size() == 2);

    check_entities(entities);
}
