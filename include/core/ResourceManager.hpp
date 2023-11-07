/*
 * This file is part of rosa.
 *
 *  rosa is free software: you can redistribute it and/or modify it under the
 * terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later
 * version.
 *
 *  rosa is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
 * A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along with
 * bbai. If not, see <https://www.gnu.org/licenses/>.
 */

#pragma once

#include "audio/AudioFile.hpp"
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <graphics/Texture.hpp>
#include <physfs.h>
#include <core/Resource.hpp>
#include <debug/Profiler.hpp>

namespace rosa {

    class ResourceManager {
        public:
            ResourceManager(ResourceManager const &) = delete;
            auto operator=(ResourceManager const &) -> ResourceManager & = delete;
            ResourceManager(ResourceManager const &&) = delete;
            auto operator=(ResourceManager const &&) -> ResourceManager & = delete;
            
            // auto getTexture(uuids::uuid uuid) -> Texture&;
            // auto getShader(uuids::uuid uuid) -> Shader&;
            // // auto getFont(uuids::uuid uuid) -> sf::Font&;
            // auto getSound(uuids::uuid uuid) -> AudioFile&;
            // auto getMusicTrack(uuids::uuid uuid) -> AudioFile&;
            // auto getScript(uuids::uuid uuid) -> const std::string&;

            template<typename T>
            auto getAsset(uuids::uuid uuid) -> T& {
                if (m_resources.contains(uuid)) {
                    T& resource = *(dynamic_cast<T*>(m_resources.at(uuid).get()));
                    return resource;
                }
            }

            static auto instance() -> ResourceManager&;
        private:
            ResourceManager();
            ~ResourceManager() = default;

            std::unordered_map<uuids::uuid, std::unique_ptr<Resource>> m_resources;
            // std::unordered_map<uuids::uuid, std::string> m_scripts;
            // std::unordered_map<uuids::uuid, AudioFile> m_sounds;
            // std::unordered_map<uuids::uuid, AudioFile> m_music_tracks;
            // std::unordered_map<uuids::uuid, Shader> m_shaders;

            //static auto populate_resource(Resource& resource) -> bool;
    };

} // namespace rosa