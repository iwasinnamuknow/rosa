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
#include <spdlog/spdlog.h>
#include <sstream>
#include <string_view>
#include <core/components/TransformComponent.hpp>
#include <core/components/SpriteComponent.hpp>

namespace YAML {

    template<>
    struct convert<sf::Vector2f> {
        static auto encode(const sf::Vector2f& rhs) -> Node {
            Node node;
            node.push_back(rhs.x);
            node.push_back(rhs.y);
            return node;
        }

        static auto decode(const Node& node, sf::Vector2f& rhs) -> bool {
            if (!node.IsSequence() || node.size() != 2) {
                return false;
            }

            rhs.x = node[0].as<float>();
            rhs.y = node[0].as<float>();
            return true;
        }
    };

    template<>
    struct convert<sf::Color> {
        static auto encode(const sf::Color& rhs) -> Node {
            Node node;
            node.push_back(rhs.r);
            node.push_back(rhs.g);
            node.push_back(rhs.b);
            node.push_back(rhs.a);
            return node;
        }

        static auto decode(const Node& node, sf::Color& rhs) -> bool {
            if (!node.IsSequence() || node.size() != 4) {
                return false;
            }

            rhs.r = node[0].as<sf::Uint8>();
            rhs.g = node[0].as<sf::Uint8>();
            rhs.b = node[0].as<sf::Uint8>();
            rhs.a = node[0].as<sf::Uint8>();
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

    auto operator<<(YAML::Emitter& out, const sf::Vector2f& vec) -> YAML::Emitter& {
        out << YAML::Flow;
        out << YAML::BeginSeq << vec.x << vec.y << YAML::EndSeq;
        return out;
    }

    auto operator<<(YAML::Emitter& out, const sf::Color& color) -> YAML::Emitter& {
        out << YAML::Flow;
        out << YAML::BeginSeq << static_cast<int>(color.r) << static_cast<int>(color.g) << static_cast<int>(color.b) << static_cast<int>(color.a) << YAML::EndSeq;
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
        out << YAML::Key << "velocity" << YAML::Value << transform.velocity;
        out << YAML::Key << "rotation" << YAML::Value << transform.rotation;
        out << YAML::EndMap; // transform
        
        if (entity.hasComponent<SpriteComponent>()) {
            auto& sprite = entity.getComponent<SpriteComponent>();
            out << YAML::BeginMap; // sprite
            out << YAML::Key << "type" << YAML::Value << "sprite";
            out << YAML::Key << "texture" << YAML::Value << uuids::to_string(sprite.m_texture_uuid);
            out << YAML::Key << "color" << YAML::Value << sprite.m_color;
            out << YAML::EndMap; // sprite
        }

        out << YAML::EndSeq; // components
        out << YAML::EndMap; // Entity
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
                    Entity new_entity = m_scene.createEntity(uuids::uuid::from_string(entity["uuid"].as<std::string>()).value());

                    if (entity["components"]) {
                        for (auto comp : entity["components"]) {
                            if (entity["type"]) {
                                auto type = comp["type"].as<std::string>();
                                if (type == "transform") {
                                    auto& transform = new_entity.getComponent<TransformComponent>();
                                    transform.position = comp["position"].as<sf::Vector2f>();
                                    transform.velocity = comp["velocity"].as<sf::Vector2f>();
                                    transform.scale = comp["scale"].as<sf::Vector2f>();
                                    transform.rotation = comp["rotation"].as<float>();
                                } else if (type == "sprite") {
                                    auto& sprite = new_entity.addComponent<SpriteComponent>();
                                    sprite.setTexture(comp["texture"].as<uuids::uuid>());
                                    sprite.setColor(comp["color"].as<sf::Color>());
                                }
                            }
                        }
                    }
                }
            }
        }

        return true;
    }

} // namespace rosa