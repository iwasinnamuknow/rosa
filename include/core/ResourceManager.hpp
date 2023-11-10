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

#include <core/Resource.hpp>
#include <debug/Profiler.hpp>
#include <graphics/Texture.hpp>
#include <audio/AudioFile.hpp>
#include <core/Uuid.hpp>

#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <physfs.h>

namespace rosa {

    class ResourceManager {
        public:
            ResourceManager(ResourceManager const &) = delete;
            auto operator=(ResourceManager const &) -> ResourceManager & = delete;
            ResourceManager(ResourceManager const &&) = delete;
            auto operator=(ResourceManager const &&) -> ResourceManager & = delete;

            template<typename T>
            auto getAsset(Uuid uuid) -> T& {
                if (m_resources.contains(uuid)) {
                    T& resource = *(dynamic_cast<T*>(m_resources.at(uuid).get()));
                    return resource;
                }
            }

            static auto instance() -> ResourceManager&;
        private:
            ResourceManager();
            ~ResourceManager() = default;

            std::unordered_map<Uuid, std::unique_ptr<Resource>> m_resources;
    };

} // namespace rosa