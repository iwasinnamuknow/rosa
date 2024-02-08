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

/*! \file */

#pragma once

#include <core/Uuid.hpp>
#include <string>

namespace rosa {

    class ResourceManager;

    /**
     * \brief Each asset will be assigned a type based on the asset manifest
     *
     * The type will affect how the asset is loaded and retrieved
     */
    enum ResourceType {
        ResourceTexture        = 0,
        ResourceSound          = 1,
        ResourceMusic          = 2,
        ResourceFont           = 3,
        ResourceScript         = 4,
        ResourceVertexShader   = 5,
        ResourceFragmentShader = 6
    };

    /**
     * \brief A requested resource was not found
     *
     * Throw when:
     *   * A resource was not found where the manifest claims
     *   * A non-existent resource is requested from the ResourceManager
     */
    class ResourceNotFoundException : public Exception {
    public:
        explicit ResourceNotFoundException(const std::string& msg)
            : Exception(msg) {
        }
    };

    /**
     * \brief Base type of any resource that can be loaded/used
     */
    class Resource {
    public:
        /**
         * \brief Base constructor
         * \param name Filename relative to the asset pack
         * \param uuid Uuid of the asset
         * \param pack Asset pack mount point
         */
        Resource(const std::string& name, const Uuid& uuid, const std::string& pack)
            : m_name(name), m_uuid(uuid), m_pack(pack) {}

        /**
         * \brief Get the filename of the asset
         */
        auto getName() const -> const std::string& {
            return m_name;
        }

        /**
         * \brief Get the Uuid of the asset relative to the pack
         */
        auto getUuid() const -> const Uuid& {
            return m_uuid;
        }

        /**
         * \brief Get the mountpoint of the asset pack
         */
        auto getPack() const -> const std::string& {
            return m_pack;
        }

        virtual ~Resource() = default;

        Resource(const Resource&)                    = delete;
        auto operator=(const Resource&) -> Resource& = delete;

        Resource(Resource&&)                    = delete;
        auto operator=(Resource&&) -> Resource& = delete;

    private:
        std::string m_name{};
        Uuid        m_uuid{};

        virtual auto closeHandles() -> void {}

        std::string m_pack{};

        friend class ResourceManager;
    };

}// namespace rosa