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
 * rosa. If not, see <https://www.gnu.org/licenses/>.
 */

#pragma once

#include <audio/AudioFile.hpp>
#include <core/Resource.hpp>
#include <core/Uuid.hpp>
#include <graphics/Texture.hpp>

#include <fmt/format.h>
#include <memory>
#include <physfs.h>
#include <string>
#include <tracy/Tracy.hpp>
#include <unordered_map>
#include <yaml-cpp/yaml.h>

namespace rosa {

    /**
     * \brief An asset pack couldn't be found
     *
     * Thrown when an attempt is made to register a non-existant asset pack
     */
    class MissingPackException : public Exception {
    public:
        explicit MissingPackException(const std::string& msg)
            : Exception(msg) {
        }
    };

    /**
     * \brief An asset pack is missing it's manifest
     *
     * Throw when there is no manifest.yaml inside the specified pack
     */
    class MissingManifestException : public Exception {
    public:
        explicit MissingManifestException(const std::string& msg)
            : Exception(msg) {
        }
    };

    /**
     * \brief An asset manifest is malformed
     *
     * Throw when there is a manifest.yaml, but it is missing one or more parts
     */
    class MalformedManifestException : public Exception {
    public:
        explicit MalformedManifestException(const std::string& msg)
            : Exception(msg) {
        }
    };

    /**
     * \brief An asset pack couldn't be unmounted
     *
     * Throw when:
     *   * Files in the pack are still open
     *   * The asset pack isn't mounted
     */
    class UnmountFailedException : public Exception {
    public:
        explicit UnmountFailedException(const std::string& msg)
            : Exception(msg) {
        }
    };

    /**
     * \brief The ResourceManager is responsible for loading any on-disk resources.
     * 
     *  It uses PhysFS as a backend, allowing for plain directories or compressed archives.
     *  It is implemented as a singleton class.
     *
     *  Once initialised, asset packs must be mounted with registerAssetPack() whereupon they asset
     *  manifest is read and the assets are loaded into memory, performing any relevant initialisation.
     *
     *  Assets can then be retrieved in the engine by Uuid with getAsset()
     */
    class ResourceManager {
    public:
        /**
         * \brief Mounts an asset location into the search path
         *
         *  Once successfully mounted, ResourceManager will attempt to load all assets in to memory.
         *
         * \param path On-disk path to the directory or archive, relative to the binary
         * \param mount_point A path to mount under, leave blank for the root path
         */
        auto registerAssetPack(const std::string& path, const std::string& mount_point) -> void;

        /**
         * \brief Unmounts a previously mounted asset location.
         *
         *  If any file handles are still open, they will be closed.
         *
         * \param path On-disk path to the directory or archive, as given in the registerAssetPack call
         */
        auto unregisterAssetPack(const std::string& path) -> void;

        /**
         * \brief Get an asset by Uuid
         *
         * Throws a ResourceNotFoundException if the asset doesn't exist.
         *
         * \tparam T The type of resource to acquire
         * \param uuid Unique identifier of the resource
         * \return A reference to the resource object
         */
        template<typename T>
        auto getAsset(Uuid uuid) const -> T& {
            ZoneScopedN("Assets:GetAsset");
            if (!m_resources.contains(uuid)) {
                throw ResourceNotFoundException(fmt::format("Couldn't locate a resource {}", uuid.toString()));
            }

            T& resource = *(dynamic_cast<T*>(m_resources.at(uuid).get()));
            return resource;
        }

        /**
         * \brief Gets the ResourceManager object
         * \return A reference to the singleton object.
         */
        static auto getInstance() -> ResourceManager&;

        /**
         * \brief Completely reset the ResourceManager instance
         *
         * Used to clean up between tests. Not used in regular operation.
         */
        static auto shutdown() -> void;

        ResourceManager(ResourceManager const&)                     = delete;
        auto operator=(ResourceManager const&) -> ResourceManager&  = delete;
        ResourceManager(ResourceManager const&&)                    = delete;
        auto operator=(ResourceManager const&&) -> ResourceManager& = delete;

        ResourceManager()  = default;
        ~ResourceManager() = default;

    private:
        std::unordered_map<Uuid, std::unique_ptr<Resource>> m_resources;
        static std::unique_ptr<ResourceManager>             s_instance;

        auto load_resource(const std::string& path, const YAML::Node& node) -> void;
    };

}// namespace rosa