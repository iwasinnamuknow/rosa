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

static auto lua_t_to_yaml(YAML::Emitter& out, sol::table& table) -> void {
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

    auto operator<<(YAML::Emitter &out, const LuaScriptComponent &component) -> YAML::Emitter & {
        out << YAML::BeginMap;
        out << YAML::Key << "type" << YAML::Value << "lua_script";
        out << YAML::Key << "script" << YAML::Value << static_cast<std::string>(component.getScriptUuid());

        // TODO There has to be a neater way of capturing data that doesn't require all data to
        //      be kept inside a single lua table
        auto table = component.getTable("persist");
        if (table.valid()) {
            out << YAML::Key << "data" << YAML::Value << YAML::BeginMap; // script data
            lua_t_to_yaml(out, table);
            out << YAML::EndMap; // script data
        }

        out << YAML::EndMap; // lua script
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