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

#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <graphics/Texture.hpp>
#include <physfs.h>
#include <core/PhysFSStream.hpp>
#include <core/Resource.hpp>
#include <debug/Profiler.hpp>

namespace rosa {

    class ResourceManager {
        public:
            ResourceManager(ResourceManager const &) = delete;
            auto operator=(ResourceManager const &) -> ResourceManager & = delete;
            ResourceManager(ResourceManager const &&) = delete;
            auto operator=(ResourceManager const &&) -> ResourceManager & = delete;
            
            auto getTexture(uuids::uuid uuid) -> Texture&;
            // auto getFont(uuids::uuid uuid) -> sf::Font&;
            // auto getSound(uuids::uuid uuid) -> sf::SoundBuffer&;
            // auto getMusicTrack(uuids::uuid uuid) -> sf::Music&;
            auto getScript(uuids::uuid uuid) -> const std::string&;

            static auto instance() -> ResourceManager&;
        private:
            ResourceManager();
            ~ResourceManager() = default;

            // std::unordered_map<std::string, sf::Texture> m_textures;
            // std::unordered_map<std::string, sf::Font> m_fonts;
            // std::unordered_map<std::string, sf::SoundBuffer> m_sounds;
            // std::unordered_map<std::string, sf::Music> m_music_tracks;

            std::unordered_map<uuids::uuid, std::unique_ptr<Resource>> m_resources;

            // template<typename T>
            // auto load_asset(const std::string& path) -> T;

            auto populate_resource(Resource& resource) -> bool;

            auto split_lines(const std::string& string, char delimiter) -> std::vector<std::string>;
    };

} // namespace rosa