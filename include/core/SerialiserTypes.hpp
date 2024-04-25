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
#include <core/components/TransformComponent.hpp>
#include <core/components/SpriteComponent.hpp>
#include <core/components/SoundPlayerComponent.hpp>
#include <core/components/MusicPlayerComponent.hpp>
#include <core/components/NativeScriptComponent.hpp>
#include <core/components/CameraComponent.hpp>

namespace rosa {
    auto operator<<(YAML::Emitter& out, const glm::vec2& vec) -> YAML::Emitter&;
    auto operator<<(YAML::Emitter& out, const glm::vec3& vec) -> YAML::Emitter&;
    auto operator<<(YAML::Emitter &out, const TransformComponent &component) -> YAML::Emitter&;
    auto operator<<(YAML::Emitter &out, const SpriteComponent &component) -> YAML::Emitter&;
    auto operator<<(YAML::Emitter &out, const SoundPlayerComponent &component) -> YAML::Emitter&;
    auto operator<<(YAML::Emitter &out, const MusicPlayerComponent &component) -> YAML::Emitter&;
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