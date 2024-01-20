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

#include "core/LuaScript.hpp"
#include <audio/AudioFile.hpp>
#include <cassert>
#include <core/Resource.hpp>
#include <core/ResourceManager.hpp>
#include <core/SerialiserTypes.hpp>
#include <core/Uuid.hpp>
#include <cstdint>
#include <exception>
#include <filesystem>
#include <fmt/format.h>
#include <graphics/Shader.hpp>
#include <memory>
#include <optional>
#include <physfs.h>
#include <spdlog/spdlog.h>
#include <sstream>
#include <string>
#include <utility>
#include <yaml-cpp/yaml.h>

#if (_WIN32)
#include <stdlib.h>
#elif (__linux__)
#include <climits>
#include <unistd.h>
#endif

auto get_exe_dir() -> std::string {
#if (_WIN32)
    char* exePath;
    if (_get_pgmptr(&exePath) != 0) {
        return {};
    }
#elif (__linux__)
    char    exePath[PATH_MAX];
    ssize_t len = ::readlink("/proc/self/exe", exePath, sizeof(exePath));
    if (len == -1 || len == sizeof(exePath)) {
        len = 0;
    }
    exePath[len] = '\0';
#endif

    std::string directory{exePath};

    return directory.substr(0, directory.find_last_of("\\/"));
}

namespace rosa {

    auto ResourceManager::registerAssetPack(const std::string& path, const std::string& mount_point) -> void {
        ZoneScopedN("Assets:MountAssetPack");

        auto exe_dir = get_exe_dir().append("/").append(path);

        PHYSFS_init(nullptr);
        int mount_success = PHYSFS_mount(exe_dir.c_str(), mount_point.c_str(), 0);
        if (mount_success == 0) {
            throw MissingPackException(fmt::format("Failed to mount {}", path));
        }

        // Attempt to read assets.lst
        if (PHYSFS_exists("manifest.yaml") == 0) {
            // doesn't exist..
            PHYSFS_unmount(exe_dir.c_str());
            throw MissingManifestException(fmt::format("Couldn't find asset manifest inside {}", path));
        }

        PHYSFS_file* file = PHYSFS_openRead(fmt::format("{}/manifest.yaml", mount_point).c_str());

        std::string buffer{};
        buffer.resize(static_cast<std::uint64_t>(PHYSFS_fileLength(file)));
        std::int64_t length_read = PHYSFS_readBytes(file, buffer.data(), static_cast<std::uint64_t>(PHYSFS_fileLength(file)));
        assert(length_read == PHYSFS_fileLength(file));

        PHYSFS_close(file);

        YAML::Node manifest = YAML::Load(buffer);

        if (!manifest["assets"]) {
            PHYSFS_unmount(exe_dir.c_str());
            throw MalformedManifestException(fmt::format("Asset manifest is malformed: {}", path));
        }

        for (const auto& assets: manifest["assets"]) {
            auto uuid     = assets["uuid"].as<Uuid>();
            auto type     = static_cast<ResourceType>(assets["type"].as<int>());
            auto filename = assets["path"].as<std::string>();

            if (type == ResourceType::ResourceTexture) {
                TextureFilterParams params;
                if (assets["filtering"]) {
                    if (assets["filtering"]["minify"]) {
                        params.minify = assets["filtering"]["minify"].as<TextureFilterMode>();
                    }

                    if (assets["filtering"]["magnify"]) {
                        params.magnify = assets["filtering"]["magnify"].as<TextureFilterMode>();
                    }
                }
                auto new_resource = std::make_unique<Texture>(filename, uuid, path, params);
                new_resource->loadFromPhysFS();
                m_resources[uuid] = std::move(new_resource);

            } else if (type == ResourceType::ResourceVertexShader) {
                auto new_resource = std::make_unique<Shader>(filename, uuid, path, ShaderType::VertexShader);
                new_resource->loadFromPhysFS();
                m_resources[uuid] = std::move(new_resource);

            } else if (type == ResourceType::ResourceFragmentShader) {
                auto new_resource = std::make_unique<Shader>(filename, uuid, path, ShaderType::FragmentShader);
                new_resource->loadFromPhysFS();
                m_resources[uuid] = std::move(new_resource);

            } else if (type == ResourceType::ResourceScript) {
                auto new_resource = std::make_unique<LuaScript>(filename, uuid, path);
                new_resource->loadFromPhysFS();
                m_resources[uuid] = std::move(new_resource);

            } else if (type == ResourceType::ResourceSound || type == ResourceType::ResourceMusic) {
                auto new_resource = std::make_unique<AudioFile>(filename, uuid, path);
                new_resource->loadFromPhysFS();
                m_resources[uuid] = std::move(new_resource);
            }
        }
    }

    auto ResourceManager::unregisterAssetPack(const std::string& path) -> void {

        ZoneScopedN("Assets:UnmountAssetPack");

        auto real_path = get_exe_dir().append("/").append(path);

        for (auto it = m_resources.cbegin(); it != m_resources.cend();) {
            if (it->second.get()->getPack() == path) {
                it->second.get()->closeHandles();
                m_resources.erase(it++);
            } else {
                ++it;
            }
        }

        if (PHYSFS_unmount(real_path.c_str()) == 0) {
            auto error = PHYSFS_getLastErrorCode();

            if (error == PHYSFS_ERR_FILES_STILL_OPEN) {
                throw UnmountFailedException("Files are still open");
            } else if (error == PHYSFS_ERR_NOT_MOUNTED) {
                throw UnmountFailedException("Pack not mounted");
            }
        }
    }

    auto ResourceManager::getInstance() -> ResourceManager& {
        if (!s_instance) {
            s_instance = std::make_unique<ResourceManager>();
        }

        return *s_instance;
    }

    auto ResourceManager::shutdown() -> void {
        if (s_instance) {
            s_instance.release();
            s_instance.reset();
        }
    }

    std::unique_ptr<ResourceManager> ResourceManager::s_instance{nullptr};

}// namespace rosa