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
#include <string>
#include <unordered_map>
#include <physfs.h>

namespace rosa {

    class MissingPackException : public Exception {
        public:
            explicit MissingPackException(const std::string& msg) : Exception(msg) {}
    };

    class MissingManifestException : public Exception {
        public:
            explicit MissingManifestException(const std::string& msg) : Exception(msg) {}
    };

    class ResourceNotFoundException : public Exception {
        public:
            explicit ResourceNotFoundException(const std::string& msg) : Exception(msg) {}
    };

    class UnmountFailedException : public Exception {
        public:
            explicit UnmountFailedException(const std::string& msg) : Exception(msg) {}
    };

    /**
     * @brief The ResourceManager is responsible for loading any on-disk resources.
     * 
     *  It uses PhysFS as a backend, allowing for plain directories or compressed archives.
     *  It is implmented as a singleton class.
     */
    class ResourceManager {
        public:
            ResourceManager(ResourceManager const &) = delete;
            auto operator=(ResourceManager const &) -> ResourceManager & = delete;
            ResourceManager(ResourceManager const &&) = delete;
            auto operator=(ResourceManager const &&) -> ResourceManager & = delete;

            /**
             * @brief Mounts an asset location into the search path
             * 
             *  Once successfully mounted, ResourceManager will attemp to load all assets.
             * 
             * @param path On-disk path to the directory or archive, relative to the binary
             * @param mount_point A path to mount under, leave blank for the root path
             */
            auto registerAssetPack(const std::string& path, const std::string& mount_point) -> void;

            /**
             * @brief Unmounts a previously mounted asset location.
             * 
             *  If any file handles are still open, they will be closed.
             * 
             * @param path On-disk path to the directory or archive, as given in the registerAssetPack call
             */
            auto unregisterAssetPack(const std::string& path) -> void;

            /**
             * @brief Get a loaded asset resource
             * 
             * @tparam T The type of resource to acquire
             * @param uuid Unique identifer of the resource
             * @return T& A reference to the resource object
             */
            template<typename T>
            auto getAsset(Uuid uuid) -> T& {
                if (m_resources.contains(uuid)) {
                    T& resource = *(dynamic_cast<T*>(m_resources.at(uuid).get()));
                    return resource;
                }
            }

            /**
             * @brief Gets the ResourceManager object
             * 
             * @return ResourceManager& A reference to the singleton object.
             */
            static auto instance() -> ResourceManager&;
        private:
            ResourceManager() = default;
            ~ResourceManager() = default;

            std::unordered_map<Uuid, std::unique_ptr<Resource>> m_resources;
    };

} // namespace rosa