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

#include <unordered_map>
#include <SFML/Graphics.hpp>
#include <physfs.h>
#include <core/PhysFSStream.hpp>

namespace rosa {

    class ResourceManager {
        public:
            ResourceManager(ResourceManager const &) = delete;
            auto operator=(ResourceManager const &) -> ResourceManager & = delete;
            ResourceManager(ResourceManager const &&) = delete;
            auto operator=(ResourceManager const &&) -> ResourceManager & = delete;
            
            auto getTexture(const std::string& path) -> sf::Texture&;

            static auto instance() -> ResourceManager&;
        private:
            ResourceManager();
            ~ResourceManager() = default;

            std::unordered_map<std::string, sf::Texture> m_textures;
            PhysFsStream m_physfs_stream;
    };

} // namespace rosa