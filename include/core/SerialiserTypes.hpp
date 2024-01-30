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

#pragma once

#include <yaml-cpp/yaml.h>
#include <glm/glm.hpp>
#include <core/Uuid.hpp>
#include <graphics/Colour.hpp>
#include <core/components/TransformComponent.hpp>
#include <core/components/SpriteComponent.hpp>
#include <core/components/SoundPlayerComponent.hpp>
#include <core/components/MusicPlayerComponent.hpp>
#include <core/components/LuaScriptComponent.hpp>
#include <core/components/NativeScriptComponent.hpp>
#include <core/components/CameraComponent.hpp>

static auto lua_node_from_yaml(const YAML::Node& node) -> sol::table;

static auto deserialise_map(const YAML::Node& node) -> sol::table {
    sol::table table{};

    for (YAML::const_iterator it = node.begin(); it != node.end(); ++it) {
        if (it->second.Type() == YAML::NodeType::Scalar) {
            const auto& tag = it->second.Tag();

            if (tag == "int") {
                table[it->first.as<std::string>()] = it->second.as<int>();
            } else if (tag == "float") {
                table[it->first.as<std::string>()] = it->second.as<float>();

            } else if (tag == "bool") {
                table[it->first.as<std::string>()] = it->second.as<bool>();

            } else if (tag == "str") {
                table[it->first.as<std::string>()] = it->second.as<std::string>();
            }
        } else {
            table[it->first.as<std::string>()] = lua_node_from_yaml(it->second);
        }
    }

    return table;
}

static auto deserialise_sequence(const YAML::Node& node) -> sol::table {
    sol::table table{};
    int index{1};

    for (YAML::const_iterator seq_it = node.begin(); seq_it != node.end(); ++seq_it) {
        if (seq_it->Type() == YAML::NodeType::Scalar) {
            const auto& tag = seq_it->Tag();

            if (tag == "int") {
                table[index++] = seq_it->as<int>();
            } else if (tag == "float") {
                table[index++] = seq_it->as<float>();

            } else if (tag == "bool") {
                table[index++] = seq_it->as<bool>();

            } else if (tag == "str") {
                table[index++] = seq_it->as<std::string>();
            }
        } else {
            table[index++] = lua_node_from_yaml(*seq_it);
        }

    }

    return table;
}

static auto lua_node_from_yaml(const YAML::Node& node) -> sol::table {
    sol::table table{};

    const auto node_type = node.Type();

    if (node_type == YAML::NodeType::Map) {
        table = deserialise_map(node);
    } else if (node_type == YAML::NodeType::Sequence) {
        table = deserialise_sequence(node);
    }

    return table;
}


namespace rosa {
    auto operator<<(YAML::Emitter& out, const glm::vec2& vec) -> YAML::Emitter&;
    auto operator<<(YAML::Emitter& out, const glm::vec3& vec) -> YAML::Emitter&;
    auto operator<<(YAML::Emitter& out, const rosa::Colour& colour) -> YAML::Emitter&;
    auto operator<<(YAML::Emitter &out, const TransformComponent &component) -> YAML::Emitter&;
    auto operator<<(YAML::Emitter &out, const SpriteComponent &component) -> YAML::Emitter&;
    auto operator<<(YAML::Emitter &out, const SoundPlayerComponent &component) -> YAML::Emitter&;
    auto operator<<(YAML::Emitter &out, const MusicPlayerComponent &component) -> YAML::Emitter&;
    auto operator<<(YAML::Emitter &out, const LuaScriptComponent &component) -> YAML::Emitter&;
    auto operator<<(YAML::Emitter &out, const NativeScriptEntity &component) -> YAML::Emitter&;
    auto operator<<(YAML::Emitter &out, const CameraComponent &component) -> YAML::Emitter&;
} // namespace rosa

namespace YAML {
    template<>
    struct convert<glm::vec2> {
        static auto decode(const Node& node, glm::vec2& rhs) -> bool {
            if (!node.IsSequence() || node.size() != 2) {
                return false;
            }

            rhs.x = node[0].as<float>();
            rhs.y = node[1].as<float>();
            return true;
        }
    };

    template<>
    struct convert<glm::vec3> {
        static auto decode(const Node& node, glm::vec3& rhs) -> bool {
            if (!node.IsSequence() || node.size() != 3) {
                return false;
            }

            rhs.x = node[0].as<float>();
            rhs.y = node[1].as<float>();
            rhs.z = node[2].as<float>();
            return true;
        }
    };

    template<>
    struct convert<rosa::Uuid> {
        static auto decode(const Node& node, rosa::Uuid& rhs) -> bool {
            rhs = node.as<std::string>();
            return true;
        }
    };

    template<>
    struct convert<rosa::Colour> {
        static auto decode(const Node& node, rosa::Colour& rhs) -> bool {
            if (!node.IsSequence() || node.size() != 4) {
                return false;
            }

            rhs.r = node[0].as<float>();
            rhs.g = node[1].as<float>();
            rhs.b = node[2].as<float>();
            rhs.a = node[3].as<float>();
            return true;
        }
    };

    template<>
    struct convert<rosa::TextureFilterMode> {
        static auto decode(const Node& node, rosa::TextureFilterMode& rhs) -> bool {
            rhs = static_cast<rosa::TextureFilterMode>(node.as<int>());
            return true;
        }
    };

    template<>
    struct convert<rosa::TransformComponent> {
        static auto decode(const Node& node, rosa::TransformComponent& rhs) -> bool {
            if (!node.IsMap()) {
                return false;
            }

            rhs.position = node["position"].as<glm::vec3>();
            rhs.scale = node["scale"].as<glm::vec3>();
            rhs.rotation = node["rotation"].as<float>();
            return true;
        }
    };

    template<>
    struct convert<rosa::SpriteComponent> {
        static auto decode(const Node& node, rosa::SpriteComponent& rhs) -> bool {
            if (!node.IsMap()) {
                return false;
            }

            rhs.setTexture(node["texture"].as<rosa::Uuid>());
            rhs.setColour(node["colour"].as<rosa::Colour>());
            return true;
        }
    };

    template<>
    struct convert<rosa::SoundPlayerComponent> {
        static auto decode(const Node& node, rosa::SoundPlayerComponent& rhs) -> bool {
            if (!node.IsMap()) {
                return false;
            }

            rhs.setAudio(node["source"].as<rosa::Uuid>());
            rhs.setDefaultVolume(node["default_volume"].as<float>());
            rhs.setPause(true);
            if (node["playing"].as<bool>()) {
                rhs.play();
            }
            rhs.setPosition(node["position"].as<double>());
            rhs.setVolume(node["volume"].as<float>());
            rhs.setPause(node["paused"].as<bool>());
            return true;
        }
    };

    template<>
    struct convert<rosa::MusicPlayerComponent> {
        static auto decode(const Node& node, rosa::MusicPlayerComponent& rhs) -> bool {
            if (!node.IsMap()) {
                return false;
            }

            rhs.setAudio(node["source"].as<rosa::Uuid>());
            rhs.setDefaultVolume(node["default_volume"].as<float>());
            rhs.setPause(true);
            if (node["playing"].as<bool>()) {
                rhs.play();
            }
            rhs.setPosition(node["position"].as<double>());
            rhs.setVolume(node["volume"].as<float>());
            rhs.setPause(node["paused"].as<bool>());
            return true;
        }
    };

    template<>
    struct convert<rosa::LuaScriptComponent> {
        static auto decode(const Node& node, rosa::LuaScriptComponent& rhs, rosa::Scene* scene, rosa::Uuid entity) -> bool {
            if (!node.IsMap()) {
                return false;
            }

            rhs.setScript(entity, scene, node["script"].as<rosa::Uuid>(), true);

            if (node["data"]) {
                auto script_data = node["data"];
                auto table_data = lua_node_from_yaml(script_data);
                rhs.setData("", table_data);
            }
            return true;
        }
    };

    template<>
    struct convert<rosa::CameraComponent> {
        static auto decode(const Node& node, rosa::CameraComponent& rhs) -> bool {
            if (!node.IsMap()) {
                return false;
            }

            rhs.setEnabled(node["enabled"].as<bool>());
            return true;
        }
    };
} // namespace YAML