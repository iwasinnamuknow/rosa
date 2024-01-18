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
#include <graphics/Shader.hpp>
#include <core/Resource.hpp>
#include <cassert>
#include <core/ResourceManager.hpp>
#include <core/Uuid.hpp>
#include <cstdint>
#include <exception>
#include <memory>
#include <optional>
#include <physfs.h>
#include <spdlog/spdlog.h>
#include <sstream>
#include <string>
#include <utility>
#include <filesystem>
#include <fmt/format.h>
#include <yaml-cpp/yaml.h>

#if (_WIN32)
#include <stdlib.h>
#elif (__linux__)
#include <unistd.h>
#include <climits>
#endif

auto get_exe_dir() -> std::string {
    #if (_WIN32)
        char *exePath;
        if (_get_pgmptr(&exePath) != 0) {
            return {};
        }
    #elif (__linux__)
        char exePath[PATH_MAX];
        ssize_t len = ::readlink("/proc/self/exe", exePath, sizeof(exePath));
        if (len == -1 || len == sizeof(exePath)) {
            len = 0;
        }
        exePath[len] = '\0';
    #endif

    std::string directory{exePath};

    return directory.substr(0, directory.find_last_of("\\/"));
}

namespace YAML {
    template<>
    struct convert<rosa::Uuid> {
        static auto decode(const Node& node, rosa::Uuid& rhs) -> bool {
            rhs = node.as<std::string>();
            return true;
        }
    };

    template<>
    struct convert<rosa::TextureFilterMode> {
        static auto decode(const Node& node, rosa::TextureFilterMode& rhs) -> bool {
            rhs = static_cast<rosa::TextureFilterMode>(node.as<int>());
            return true;
        }
    };
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
        if (PHYSFS_exists("manifest.yaml") != 0) {
            PHYSFS_file* myfile = PHYSFS_openRead(fmt::format("{}/manifest.yaml", mount_point).c_str());
            std::string buffer{};
            buffer.resize(PHYSFS_fileLength(myfile));
            std::int64_t length_read = PHYSFS_readBytes(myfile, buffer.data(), static_cast<std::uint64_t>(PHYSFS_fileLength(myfile)));
            assert(length_read == PHYSFS_fileLength(myfile));
            PHYSFS_close(myfile);

            YAML::Node manifest = YAML::Load(buffer);

            if (manifest["assets"]) {
                for (const auto& a : manifest["assets"]) {
                    auto uuid = Uuid(a["uuid"].as<Uuid>());
                    auto type = static_cast<resource_type>(a["type"].as<int>());
                    auto filename = a["path"].as<std::string>();

                    if (type == resource_type::resource_texture) {
                        TextureFilterParams params;
                        if (a["filtering"]) {
                            if (a["filtering"]["minify"]) {
                                params.minify = a["filtering"]["minify"].as<TextureFilterMode>();
                            }

                            if (a["filtering"]["magnify"]) {
                                params.magnify = a["filtering"]["magnify"].as<TextureFilterMode>();
                            }
                        }
                        auto new_resource = std::make_unique<Texture>( filename, uuid, path, params);
                        new_resource->loadFromPhysFS();
                        m_resources[uuid] = std::move(new_resource);
                    
                    } else if (type == resource_type::resource_vertex_shader) {
                        auto new_resource = std::make_unique<Shader>( filename, uuid, path, ShaderType::VertexShader);
                        new_resource->loadFromPhysFS();
                        m_resources[uuid] = std::move(new_resource);

                    } else if (type == resource_type::resource_fragment_shader) {
                        auto new_resource = std::make_unique<Shader>( filename, uuid, path, ShaderType::FragmentShader);
                        new_resource->loadFromPhysFS();
                        m_resources[uuid] = std::move(new_resource);

                    } else if (type == resource_type::resource_script) {
                        auto new_resource = std::make_unique<LuaScript>( filename, uuid, path);
                        new_resource->loadFromPhysFS();
                        m_resources[uuid] = std::move(new_resource);

                    } else if (type == resource_type::resource_sound || type == resource_type::resource_music) {
                        auto new_resource = std::make_unique<AudioFile>( filename, uuid, path);
                        new_resource->loadFromPhysFS();
                        m_resources[uuid] = std::move(new_resource);
                    }
                }
            }

        } else {
            // doesn't exists..
            PHYSFS_unmount(exe_dir.c_str());
            throw MissingManifestException(fmt::format("Couldn't find asset manifest inside {}", path));
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

} // namespace rosa