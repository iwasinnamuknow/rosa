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

#include <core/SerialiserTypes.hpp>

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
        out << YAML::BeginSeq << colour.r << colour.g << colour.b << colour.a << YAML::EndSeq;
        return out;
    }

    auto operator<<(YAML::Emitter &out, const TransformComponent &component) -> YAML::Emitter & {
        out << YAML::BeginMap;
        out << YAML::Key << "type" << YAML::Value << "transform";
        out << YAML::Key << "position" << YAML::Value << component.position;
        out << YAML::Key << "scale" << YAML::Value << component.scale;
        out << YAML::Key << "rotation" << YAML::Value << component.rotation;
        out << YAML::EndMap;
        return out;
    }

    auto operator<<(YAML::Emitter &out, const SpriteComponent &component) -> YAML::Emitter & {
        out << YAML::BeginMap;
        out << YAML::Key << "type" << YAML::Value << "sprite";
        out << YAML::Key << "texture" << YAML::Value << static_cast<std::string>(component.getTextureUuid());
        out << YAML::Key << "colour" << YAML::Value << component.getColour();
        out << YAML::EndMap;
        return out;
    }

    auto operator<<(YAML::Emitter &out, const SoundPlayerComponent &component) -> YAML::Emitter & {
        out << YAML::BeginMap;
        out << YAML::Key << "type" << YAML::Value << "sound";
        out << YAML::Key << "source" << YAML::Value << static_cast<std::string>(component.getAudioUuid());
        out << YAML::Key << "position" << YAML::Value << component.getPosition();
        out << YAML::Key << "volume" << YAML::Value << component.getVolume();
        out << YAML::Key << "default_volume" << YAML::Value << component.getDefaultVolume();
        out << YAML::Key << "paused" << YAML::Value << component.getPause();
        out << YAML::Key << "playing" << YAML::Value << component.isPlaying();
        out << YAML::EndMap;
        return out;
    }

    auto operator<<(YAML::Emitter &out, const MusicPlayerComponent &component) -> YAML::Emitter & {
        out << YAML::BeginMap;
        out << YAML::Key << "type" << YAML::Value << "music";
        out << YAML::Key << "source" << YAML::Value << static_cast<std::string>(component.getAudioUuid());
        out << YAML::Key << "position" << YAML::Value << component.getPosition();
        out << YAML::Key << "volume" << YAML::Value << component.getVolume();
        out << YAML::Key << "default_volume" << YAML::Value << component.getDefaultVolume();
        out << YAML::Key << "paused" << YAML::Value << component.getPause();
        out << YAML::Key << "playing" << YAML::Value << component.isPlaying();
        out << YAML::EndMap;
        return out;
    }

    auto operator<<(YAML::Emitter &out, const NativeScriptEntity &component) -> YAML::Emitter & {
        out << YAML::BeginMap; // nsc
        out << YAML::Key << "type" << YAML::Value << "native_script";
        out << YAML::Key << "data" << YAML::Value << component.serialise();
        out << YAML::Key << "script" << YAML::Value << component.getName();
        out << YAML::EndMap; // nsc
        return out;
    }

    auto operator<<(YAML::Emitter &out, const CameraComponent &component) -> YAML::Emitter & {
        out << YAML::BeginMap;
        out << YAML::Key << "type" << YAML::Value << "camera";
        out << YAML::Key << "enabled" << YAML::Value << component.getEnabled();
        out << YAML::EndMap;
        return out;
    }

} // namespace rosa