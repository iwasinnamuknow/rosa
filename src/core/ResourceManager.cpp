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
#include <graphics/BitmapFont.hpp>
#include <graphics/Shader.hpp>
#include <physfs.h>
#include <string>

#if defined(_WIN32)
#include <stdlib.h>
#elif defined(__linux__)
#include <unistd.h>
#endif

auto get_exe_dir() -> std::string {
#if defined(_WIN32)
    char* exe_path;
    if (_get_pgmptr(&exe_path) != 0) {
        return {};
    }

    std::string directory{exe_path};
#elif defined(__linux__)
    std::uint64_t     path_max{256};
    std::vector<char> exe_path(path_max, 0);
    ssize_t           len = ::readlink("/proc/self/exe", exe_path.data(), /*max_length=*/path_max);

    if (len == -1 && len < static_cast<ssize_t>(path_max)) {
        return {};
    }

    std::string directory(exe_path.begin(), exe_path.end());
#endif

    return directory.substr(0, directory.find_last_of("\\/"));
}

namespace rosa {

    auto ResourceManager::registerAssetPack(const std::string& path, const std::string& mount_point) -> void {
        ZoneScopedN("Assets:MountAssetPack");

        spdlog::debug("ResourceManager: registering asset pack {} to /{}", path, mount_point);

        auto exe_dir = get_exe_dir().append("/").append(path);

        PHYSFS_init(nullptr);
        int mount_success = PHYSFS_mount(exe_dir.c_str(), mount_point.c_str(), 0);
        if (mount_success == 0) {
            throw MissingPackException(fmt::format("Failed to mount {}", path));
        }

        // Attempt to read assets.lst
        if (PHYSFS_exists("manifest.yaml") == 0) {
            // doesn't exist...
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

        for (const auto& asset: manifest["assets"]) {
            load_resource(path, asset);
        }
    }

    auto ResourceManager::unregisterAssetPack(const std::string& path) -> void {

        ZoneScopedN("Assets:UnmountAssetPack");

        auto real_path = get_exe_dir().append("/").append(path);

        for (auto it = m_resources.cbegin(); it != m_resources.cend();) {
            if (it->second->getPack() == path) {
                it->second->closeHandles();
                m_resources.erase(it++);
            } else {
                ++it;
            }
        }

        if (PHYSFS_unmount(real_path.c_str()) == 0) {
            auto error = PHYSFS_getLastErrorCode();

            //            if (error == PHYSFS_ERR_FILES_STILL_OPEN) {
            //                throw UnmountFailedException("Files are still open");
            //            }

            if (error == PHYSFS_ERR_NOT_MOUNTED) {
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
        spdlog::debug("ResourceManager: Shutting down");
        if (s_instance) {
            //s_instance.release();
            s_instance.reset();
        }
    }

    std::unique_ptr<ResourceManager> ResourceManager::s_instance{nullptr};

    auto ResourceManager::load_resource(const std::string& path, const YAML::Node& node) -> void {
        auto uuid     = node["uuid"].as<Uuid>();
        auto type     = static_cast<ResourceType>(node["type"].as<int>());
        auto filename = node["path"].as<std::string>();

        switch (type) {
            case ResourceType::ResourceTexture: {
                TextureFilterParams params;
                if (node["filtering"]) {
                    if (node["filtering"]["minify"]) {
                        params.minify = node["filtering"]["minify"].as<TextureFilterMode>();
                    }

                    if (node["filtering"]["magnify"]) {
                        params.magnify = node["filtering"]["magnify"].as<TextureFilterMode>();
                    }
                }

                spdlog::debug(
                        "ResourceManager: Loading texture {} from /{} (filter min: {:#x}, filter mag: {:#x})",
                        uuid.toString(), filename, static_cast<int>(params.minify), static_cast<int>(params.magnify));
                m_resources[uuid] = std::make_unique<Texture>(filename, uuid, path, params);
            } break;
            case ResourceType::ResourceVertexShader:
                spdlog::debug("ResourceManager: Loading vertex shader {} from /{}", uuid.toString(), filename);
                m_resources[uuid] = std::make_unique<Shader>(filename, uuid, path, ShaderType::VertexShader);
                break;
            case ResourceType::ResourceFragmentShader:
                spdlog::debug("ResourceManager: Loading fragment shader {} from /{}", uuid.toString(), filename);
                m_resources[uuid] = std::make_unique<Shader>(filename, uuid, path, ShaderType::FragmentShader);
                break;
                //            case ResourceType::ResourceScript:
                //                spdlog::debug("ResourceManager: Loading lua script {} from /{}", uuid.toString(), filename);
                //                m_resources[uuid] = std::make_unique<LuaScript>(filename, uuid, path);
                //                break;
            case ResourceType::ResourceSound:
            case ResourceType::ResourceMusic:
                spdlog::debug("ResourceManager: Loading audio track {} from /{}", uuid.toString(), filename);
                m_resources[uuid] = std::make_unique<AudioFile>(filename, uuid, path);
                break;
            case ResourceType::ResourceFont:
                spdlog::debug("ResourceManager: Loading font {} from /{}", uuid.toString(), filename);
                m_resources[uuid] = std::make_unique<BitmapFont>(filename, uuid, path);
                break;
        }
    }

}// namespace rosa