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

#include <core/Scene.hpp>
#include <core/SceneSerialiser.hpp>
#include <fstream>
#include <limits>
#include <spdlog/spdlog.h>
#include <sstream>
#include <string_view>
#include <core/components/TransformComponent.hpp>
#include <core/components/SpriteComponent.hpp>
#include <core/components/LuaScriptComponent.hpp>
#include <graphics/Colour.hpp>
#include <cmath>
#include <core/Uuid.hpp>

namespace YAML {

    template<>
    struct convert<glm::vec2> {
        static auto encode(const glm::vec2& rhs) -> Node {
            Node node;
            node.push_back(rhs.x);
            node.push_back(rhs.y);
            return node;
        }

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
        static auto encode(const glm::vec3& rhs) -> Node {
            Node node;
            node.push_back(rhs.x);
            node.push_back(rhs.y);
            node.push_back(rhs.z);
            return node;
        }

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
    struct convert<rosa::Colour> {
        static auto encode(const rosa::Colour& rhs) -> Node {
            Node node;
            node.push_back(rhs.r);
            node.push_back(rhs.g);
            node.push_back(rhs.b);
            node.push_back(rhs.a);
            return node;
        }

        static auto decode(const Node& node, rosa::Colour& rhs) -> bool {
            if (!node.IsSequence() || node.size() != 4) {
                return false;
            }

            rhs.r = node[0].as<std::uint8_t>();
            rhs.g = node[1].as<std::uint8_t>();
            rhs.b = node[2].as<std::uint8_t>();
            rhs.a = node[3].as<std::uint8_t>();
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

} // namespace YAML

namespace rosa {

    auto operator<<(YAML::Emitter& out, const glm::vec2& vec) -> YAML::Emitter& {
        out << YAML::Flow;
        out << YAML::BeginSeq << vec.x << vec.y << YAML::EndSeq;
        return out;
    }

    auto operator<<(YAML::Emitter& out, const glm::vec3& vec) -> YAML::Emitter& {
        out << YAML::Flow;
        out << YAML::BeginSeq << vec.x << vec.y << vec.z << YAML::EndSeq;
        return out;
    }

    auto operator<<(YAML::Emitter& out, const rosa::Colour& colour) -> YAML::Emitter& {
        out << YAML::Flow;
        out << YAML::BeginSeq << static_cast<int>(colour.r) << static_cast<int>(colour.g) << static_cast<int>(colour.b) << static_cast<int>(colour.a) << YAML::EndSeq;
        return out;
    }

    SceneSerialiser::SceneSerialiser(Scene& scene) : m_scene(scene) {}

    auto SceneSerialiser::serialiseToYaml(const std::string& filepath) -> void {
        YAML::Emitter out;

        out << YAML::BeginMap; // Main
            out << YAML::Key << "version" << YAML::Value << serialiser_version;
            out << YAML::Key << "scene" << YAML::Value << YAML::BeginMap; // Scene
                out << YAML::Key << "entities" << YAML::Value << YAML::BeginSeq; // Entities
                    for (auto& [eid, entity] : m_scene.m_entities) {
                        serialise_entity(out, entity);
                    }
                out << YAML::EndSeq; // Entities
            out << YAML::EndMap; // Scene
        out << YAML::EndMap; // Main

        std::ofstream fout(filepath);
        fout << out.c_str();
    }

    auto SceneSerialiser::serialise_entity(YAML::Emitter& out, Entity& entity) -> void {
        out << YAML::BeginMap; // Entity
        //out << YAML::Key << "entt_id" << YAML::Value << static_cast<int>(entity.getId());
        out << YAML::Key << "uuid" << YAML::Value << static_cast<std::string>(entity.getUUID());
        out << YAML::Key << "components" << YAML::Value << YAML::BeginSeq; //components

        auto& transform = entity.getComponent<TransformComponent>();
        out << YAML::BeginMap; // transform
        out << YAML::Key << "type" << YAML::Value << "transform";
        out << YAML::Key << "position" << YAML::Value << transform.position;
        out << YAML::Key << "scale" << YAML::Value << transform.scale;
        out << YAML::Key << "rotation" << YAML::Value << transform.rotation;
        out << YAML::EndMap; // transform
        
        if (entity.hasComponent<SpriteComponent>()) {
            auto& sprite = entity.getComponent<SpriteComponent>();
            out << YAML::BeginMap; // sprite
            out << YAML::Key << "type" << YAML::Value << "sprite";
            out << YAML::Key << "texture" << YAML::Value << static_cast<std::string>(sprite.getTextureUUID());
            out << YAML::Key << "color" << YAML::Value << sprite.getColour();
            out << YAML::EndMap; // sprite
        }

        if (entity.hasComponent<SoundPlayerComponent>()) {
            auto& player = entity.getComponent<SoundPlayerComponent>();
            out << YAML::BeginMap; // sound player
            out << YAML::Key << "type" << YAML::Value << "sound";
            out << YAML::Key << "source" << YAML::Value << static_cast<std::string>(player.m_uuid);
            out << YAML::Key << "position" << YAML::Value << player.getPosition();
            out << YAML::Key << "volume" << YAML::Value << player.m_volume;
            out << YAML::Key << "default_volume" << YAML::Value << player.m_default_volume;
            out << YAML::Key << "paused" << YAML::Value << player.getPause();
            out << YAML::Key << "playing" << YAML::Value << player.isPlaying();
            out << YAML::EndMap;
        }

        if (entity.hasComponent<MusicPlayerComponent>()) {
            auto& player = entity.getComponent<MusicPlayerComponent>();
            out << YAML::BeginMap; // sound player
            out << YAML::Key << "type" << YAML::Value << "music";
            out << YAML::Key << "source" << YAML::Value << static_cast<std::string>(player.m_uuid);
            out << YAML::Key << "position" << YAML::Value << player.getPosition();
            out << YAML::Key << "volume" << YAML::Value << player.m_volume;
            out << YAML::Key << "default_volume" << YAML::Value << player.m_default_volume;
            out << YAML::Key << "paused" << YAML::Value << player.getPause();
            out << YAML::Key << "playing" << YAML::Value << player.isPlaying();
            out << YAML::EndMap;
        }

        if (entity.hasComponent<LuaScriptComponent>()) {
            auto& lsc = entity.getComponent<LuaScriptComponent>();
            out << YAML::BeginMap; // lua script
            out << YAML::Key << "type" << YAML::Value << "lua_script";
            out << YAML::Key << "script" << YAML::Value << static_cast<std::string>(lsc.m_uuid);

            // TODO There has to be a neater way of capturing data that doesn't require all data to
            //      be kept inside a single lua table
            sol::table table = lsc.m_state["persist"];
            if (table.valid()) {
                out << YAML::Key << "data" << YAML::Value << YAML::BeginMap; // script data

                lua_t_to_yaml(out, table);

                out << YAML::EndMap; // script data
            }

            out << YAML::EndMap; // lua script
        }

        out << YAML::EndSeq; // components
        out << YAML::EndMap; // Entity
    }

    auto SceneSerialiser::lua_t_to_yaml(YAML::Emitter& out, sol::table& table) -> void {
        for (auto it = table.begin(); it != table.end(); ++it) {
            auto [key, val] = *it;

            out << YAML::Key << key.as<std::string>();
            out << YAML::Value;

            sol::type type = val.get_type();

            if (type == sol::type::number) {
                const double number = val.as<double>();
                auto floored = std::floor(number);
                if (floored < number) {
                    out << YAML::LocalTag("float") << number;
                } else {
                    out << YAML::LocalTag("int") << number;
                } 
            } else if (type == sol::type::boolean) {
                const bool boolean = val.as<bool>();
                out << YAML::LocalTag("bool") << boolean;
            } else if (type == sol::type::string) {
                const std::string str = val.as<std::string>();
                out << YAML::LocalTag("str") << str;
            } else if (type == sol::type::table) {
                sol::table subt = val;
                lua_t_to_yaml(out, subt);
            }
        }
    }

    auto SceneSerialiser::deserialiseFromYaml(const std::string& filepath) -> bool {
        YAML::Node data = YAML::LoadFile(filepath);

        if (!data["version"]) {
            spdlog::critical("Failed to deserialise scene, no version header.");
            abort();
        }

        if (data["version"].as<int>() != serialiser_version) {
            spdlog::critical("Failed to deserialise scene, version mismatch.");
            abort();
        }

        if (data["scene"]) {
            auto scene = data["scene"];

            if (scene["entities"]) {

                auto entities = scene["entities"];
                for (auto entity: entities) {
                    if (entity["uuid"]) {
                        Entity new_entity = m_scene.create_entity(Uuid(entity["uuid"].as<std::string>()));

                        if (entity["components"]) {
                            for (auto comp : entity["components"]) {
                                if (comp["type"]) {
                                    auto type = comp["type"].as<std::string>();
                                    if (type == "transform") {
                                        auto& transform = new_entity.getComponent<TransformComponent>();
                                        transform.position = comp["position"].as<glm::vec3>();
                                        transform.scale = comp["scale"].as<glm::vec3>();
                                        transform.rotation = comp["rotation"].as<float>();
                                    } else if (type == "sprite") {
                                        auto& sprite = new_entity.addComponent<SpriteComponent>();
                                        sprite.setTexture(comp["texture"].as<std::string>());
                                        sprite.setColour(comp["color"].as<rosa::Colour>());
                                    } else if (type == "lua_script") {
                                        auto& lsc = new_entity.addComponent<LuaScriptComponent>(&m_scene, new_entity);
                                        auto script_uuid = comp["script"].as<std::string>();

                                        auto uuid = Uuid(script_uuid);

                                        lsc.setScript(uuid, true);

                                        if (comp["data"]) {
                                            auto script_data = comp["data"];
                                            auto table_data = lua_node_from_yaml(script_data);
                                            lsc.set_data("", table_data);
                                        }

                                    } else if (type == "sound") {
                                        auto& player = new_entity.addComponent<SoundPlayerComponent>();
                                        auto uuid = Uuid(comp["source"].as<std::string>());
                                        player.setAudio(uuid);
                                        player.setDefaultVolume(comp["default_volume"].as<float>());
                                        player.play();
                                        player.setPause(true);
                                        player.setPosition(comp["position"].as<double>());
                                        auto paused = comp["paused"].as<bool>();
                                        auto playing = comp["playing"].as<bool>();
                                        if (paused == false && playing == true) {
                                            player.setPause(false);
                                        }
                                        player.setVolume(comp["volume"].as<float>());
                                    } else if (type == "music") {
                                        auto& player = new_entity.addComponent<MusicPlayerComponent>();
                                        auto uuid = Uuid(comp["source"].as<std::string>());
                                        player.setAudio(uuid);
                                        auto pos = comp["position"].as<double>();
                                        player.setDefaultVolume(comp["default_volume"].as<float>());
                                        player.play();
                                        player.setPause(true);
                                        player.setPosition(pos);
                                        auto paused = comp["paused"].as<bool>();
                                        auto playing = comp["playing"].as<bool>();
                                        if (paused == false && playing == true) {
                                            player.setPause(false);
                                        }
                                        player.setVolume(comp["volume"].as<float>());
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }

        // out << YAML::Key << "position" << YAML::Value << player.getPosition();
        // out << YAML::Key << "volume" << YAML::Value << player.m_volume;
        // out << YAML::Key << "default_volume" << YAML::Value << player.m_default_volume;
        // out << YAML::Key << "paused" << YAML::Value << player.getPause();
        // out << YAML::Key << "playing" << YAML::Value << player.isPlaying();

        return true; // TODO this is pointless, we need to check something
    }

    auto SceneSerialiser::lua_node_from_yaml(const YAML::Node& node) -> sol::table {
        sol::table table{};

        const auto node_type = node.Type();

        if (node_type == YAML::NodeType::Map) {
            table = deserialise_map(node);
        } else if (node_type == YAML::NodeType::Sequence) {
            table = deserialise_sequence(node);
        }

        return table;
    }

    auto SceneSerialiser::deserialise_map(const YAML::Node& node) -> sol::table {
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

    auto SceneSerialiser::deserialise_sequence(const YAML::Node& node) -> sol::table {
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

} // namespace rosa