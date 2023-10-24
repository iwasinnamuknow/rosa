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
    struct convert<uuids::uuid> {
        static auto decode(const Node& node, uuids::uuid& rhs) -> bool {
            rhs = uuids::uuid::from_string(node.as<std::string>()).value();
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
        out << YAML::Key << "uuid" << YAML::Value << entity.getUUID();
        out << YAML::Key << "components" << YAML::Value << YAML::BeginSeq; //components

        auto& transform = entity.getComponent<TransformComponent>();
        out << YAML::BeginMap; // transform
        out << YAML::Key << "type" << YAML::Value << "transform";
        out << YAML::Key << "position" << YAML::Value << transform.position;
        out << YAML::Key << "scale" << YAML::Value << transform.scale;
        out << YAML::EndMap; // transform
        
        if (entity.hasComponent<SpriteComponent>()) {
            auto& sprite = entity.getComponent<SpriteComponent>();
            out << YAML::BeginMap; // sprite
            out << YAML::Key << "type" << YAML::Value << "sprite";
            out << YAML::Key << "texture" << YAML::Value << uuids::to_string(sprite.m_texture_uuid);
            out << YAML::Key << "color" << YAML::Value << sprite.getColour();
            out << YAML::EndMap; // sprite
        }

        if (entity.hasComponent<LuaScriptComponent>()) {
            auto& lsc = entity.getComponent<LuaScriptComponent>();
            out << YAML::BeginMap; // lua script
            out << YAML::Key << "type" << YAML::Value << "lua_script";
            out << YAML::Key << "script" << YAML::Value << uuids::to_string(lsc.m_uuid);

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
            auto entities = data["entities"];
            for (auto entity : entities) {
                if (entity["uuid"]) {
                    Entity new_entity = m_scene.create_entity(uuids::uuid::from_string(entity["uuid"].as<std::string>()).value());

                    if (entity["components"]) {
                        for (auto comp : entity["components"]) {
                            if (entity["type"]) {
                                auto type = comp["type"].as<std::string>();
                                if (type == "transform") {
                                    auto& transform = new_entity.getComponent<TransformComponent>();
                                    transform.position = comp["position"].as<glm::vec3>();
                                    transform.scale = comp["scale"].as<glm::vec3>();
                                } else if (type == "sprite") {
                                    auto& sprite = new_entity.addComponent<SpriteComponent>();
                                    sprite.setTexture(comp["texture"].as<uuids::uuid>());
                                    sprite.setColour(comp["color"].as<rosa::Colour>());
                                } else if (type == "lua_script") {
                                    auto& lsc = new_entity.addComponent<LuaScriptComponent>(m_scene, new_entity);
                                    auto script_uuid = comp["script"].as<std::string>();

                                    auto uuid = uuids::uuid::from_string(script_uuid);
                                    if (uuid.has_value()) {

                                        lsc.setScript(uuid.value());

                                        if (comp["data"]) {
                                            auto script_data = comp["data"];
                                            auto table_data = lua_t_from_yaml(script_data);
                                            lsc.set_data("", table_data);
                                        }
                                    }
                                    //lsc.setScript(comp["script"].as<uuids::uuid>(), );
                                    //sprite.setColor(comp["color"].as<sf::Color>());
                                    
                                }
                            }
                        }
                    }
                }
            }
        }

        return true; // TODO this is pointless, we need to check something
    }

    auto SceneSerialiser::lua_t_from_yaml(const YAML::Node& node) -> sol::table {
        // sol::table table{};

        // const auto node_type = node.Type();

        // if (node_type == YAML::NodeType::Map) {
        //     table = deserialise_map(node);

        // } else if (node_type == YAML::NodeType::Sequence) {
        //     table = deserialise_sequence(node);

        // } else if (node_type == YAML::NodeType::Scalar) {
        //     const auto& tag = node.Tag();

        //     if (tag == "int") {
        //         table[node] = it->second.as<int>();

        //     } else if (it->second.Tag() == "float") {
        //         table[it->first.as<std::string>()] = it->second.as<float>();

        //     } else if (it->second.Tag() == "bool") {
        //         table[it->first.as<std::string>()] = it->second.as<bool>();

        //     } else if (it->second.Tag() == "str") {
        //         table[it->first.as<std::string>()] = it->second.as<std::string>();
        //     }
        //     table = deserialise_scalar(node);
        // }

        // return table;
    }

    auto SceneSerialiser::deserialise_map(const YAML::Node& node) -> sol::table {
        sol::table table{};

        for (YAML::const_iterator it = node.begin(); it != node.end(); ++it) {
            table[it->first.as<std::string>()] = lua_t_from_yaml(it->second);
        }

        return table;
    }

    auto SceneSerialiser::deserialise_sequence(const YAML::Node& node) -> sol::table {
        sol::table table{};
        int index{0};

        for (YAML::const_iterator seq_it = node.begin(); seq_it != node.end(); ++seq_it) {
            table[index++] = lua_t_from_yaml(*seq_it);
        }

        return table;
    }

    template<typename T>
    auto SceneSerialiser::deserialise_scalar(const YAML::Node& node) -> T {
        
    }

} // namespace rosa