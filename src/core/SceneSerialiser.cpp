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

#include "core/Scene.hpp"
#include "core/components/SpriteComponent.hpp"
#include "core/components/TransformComponent.hpp"
#include <SFML/System/Vector2.hpp>
#include <core/SceneSerialiser.hpp>
#include <fstream>
#include <uuid.h>
#include <yaml-cpp/emittermanip.h>

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
        out << YAML::Key << "Version" << YAML::Value << serialiser_version;
        out << YAML::Key << "Scene" << YAML::Value << YAML::BeginMap; // Scene
        out << YAML::Key << "Entities" << YAML::Value << YAML::BeginSeq; // Entities
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
        out << YAML::Key << "entt_id" << YAML::Value << static_cast<int>(entity.getId());
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
        return false;
    }

} // namespace rosa