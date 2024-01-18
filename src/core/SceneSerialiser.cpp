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
#include <core/components/SoundPlayerComponent.hpp>
#include <core/components/MusicPlayerComponent.hpp>
#include <core/components/NativeScriptComponent.hpp>
#include <core/components/CameraComponent.hpp>
#include <graphics/Colour.hpp>
#include <cmath>
#include <core/Uuid.hpp>
#include <core/SerialiserTypes.hpp>

namespace rosa {

    SceneSerialiser::SceneSerialiser(Scene& scene) : m_scene(scene) {}

    auto SceneSerialiser::registerNSC(const std::string &name, NativeScriptEntity *(*factory_func)(rosa::Scene *, rosa::Entity *)) -> void {
        m_nsc_map.insert(std::make_pair(std::string(name), factory_func));
    }

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
            out << YAML::Key << "uuid" << YAML::Value << static_cast<std::string>(entity.getUUID());
            out << YAML::Key << "components" << YAML::Value << YAML::BeginSeq; //components

                auto& transform = entity.getComponent<TransformComponent>();
                out << transform;

                if (entity.hasComponent<SpriteComponent>()) {
                    auto& sprite = entity.getComponent<SpriteComponent>();
                    out << sprite;
                }

                if (entity.hasComponent<SoundPlayerComponent>()) {
                    auto& player = entity.getComponent<SoundPlayerComponent>();
                    out << player;
                }

                if (entity.hasComponent<MusicPlayerComponent>()) {
                    auto& player = entity.getComponent<MusicPlayerComponent>();
                    out << player;
                }

                if (entity.hasComponent<LuaScriptComponent>()) {
                    auto& lsc = entity.getComponent<LuaScriptComponent>();
                    out << lsc;
                }

                if (entity.hasComponent<NativeScriptComponent>()) {
                    auto* nsc = entity.getComponent<NativeScriptComponent>().instance;
                    out << *nsc;
                }

                if (entity.hasComponent<CameraComponent>()) {
                    auto& camera = entity.getComponent<CameraComponent>();
                    out << camera;
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

        if (!data["scene"]) {
            spdlog::critical("No scene to deserialise.");
            abort();
        }

        auto scene = data["scene"];
        if (!scene["entities"]) {
            spdlog::critical("No scene entities to deserialise.");
            abort();
        }

        auto entities = scene["entities"];
        for (auto entity: entities) {
            if (!entity["uuid"]) {
                spdlog::critical("Missing UUID for serialised entity.");
                abort();
            }

            if (!entity["components"]) {
                spdlog::critical("Missing components for serialised entity.");
                abort();
            }

            Entity new_entity = m_scene.createEntity(Uuid(entity["uuid"].as<std::string>()));

            for (auto comp : entity["components"]) {
                if (!comp["type"]) {
                    spdlog::critical("Missing component type for serialised entity.");
                    abort();
                }

                auto type = comp["type"].as<std::string>();

                if (type == "transform") {
                    auto& transform = new_entity.getComponent<TransformComponent>();
                    YAML::convert<TransformComponent>::decode(comp, transform);
                } else if (type == "sprite") {
                    auto& sprite = new_entity.addComponent<SpriteComponent>();
                    sprite = comp.as<SpriteComponent>();
                } else if (type == "lua_script") {
                    auto& lsc = new_entity.addComponent<LuaScriptComponent>(&m_scene, new_entity);
                    YAML::convert<LuaScriptComponent>::decode(comp, lsc);
                } else if (type == "sound") {
                    auto& player = new_entity.addComponent<SoundPlayerComponent>();
                    YAML::convert<SoundPlayerComponent>::decode(comp, player);
                } else if (type == "music") {
                    auto& player = new_entity.addComponent<MusicPlayerComponent>();
                    YAML::convert<MusicPlayerComponent>::decode(comp, player);
                } else if (type == "native_script") {
                    auto classname = comp["script"].as<std::string>();
                    auto factory_func = m_nsc_map.at(classname);
                    auto nsc = factory_func(&m_scene, &new_entity);
                    nsc->deserialise(comp["data"]);
                    m_scene.getRegistry().emplace<NativeScriptComponent>(new_entity).bind(nsc);
                } else if (type == "camera") {
                    auto& camera = new_entity.addComponent<CameraComponent>();
                    YAML::convert<CameraComponent>::decode(comp, camera);
                }
            }
        }

        return true;
    }

} // namespace rosa