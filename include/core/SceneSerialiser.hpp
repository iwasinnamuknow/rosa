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

#pragma once

#include <core/Scene.hpp>
#include <yaml-cpp/yaml.h>
#include <sol/sol.hpp>

namespace rosa {

    constexpr int serialiser_version{1};

    class SceneSerialiser {
        public:
            explicit SceneSerialiser(Scene& scene);

            auto serialiseToYaml(const std::string& filepath) -> void;
            auto deserialiseFromYaml(const std::string& filepath) -> bool;

        private:
            auto serialise_entity(YAML::Emitter& out, Entity& entity) -> void;
            auto lua_t_to_yaml(YAML::Emitter& out, sol::table& table) -> void;
            Scene& m_scene;
    };

} // namespace rosa