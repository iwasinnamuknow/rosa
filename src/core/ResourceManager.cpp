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

#include "audio/AudioFile.hpp"
#include <core/Resource.hpp>
#include <cassert>
#include <core/ResourceManager.hpp>
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

#if (_WIN32)
#include <stdlib.h>
#elif (__linux__)
#include <unistd.h>
#include <climits>
#endif

auto split_lines(const std::string& string, char delimiter) -> std::vector<std::string> {
    auto result = std::vector<std::string>{};
    auto stream = std::stringstream(string);

    for (std::string line; std::getline(stream, line, delimiter);) {
        result.push_back(line);
    }

    return result;
}

auto get_exe_dir() -> std::string {
    #if (_WIN32)
        char *exePath;
        if (_get_pgmptr(&exePath) != 0) {
            exePath = "";
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

namespace rosa {

    ResourceManager::ResourceManager() {
        ROSA_PROFILE_SCOPE("Assets:ParseAssetList");

        PHYSFS_init(nullptr);
        int mount_success = PHYSFS_mount(
            std::format("{}{}base.pak", get_exe_dir(), std::filesystem::path::preferred_separator).data(), "", 1);
        if (mount_success == 0) {
            throw Exception("Failed to locate base.pak");
        }

        // Attempt to read assets.lst
        if (PHYSFS_exists("assets.lst") != 0) {
            PHYSFS_file* myfile = PHYSFS_openRead("assets.lst");
            std::string buffer{};
            buffer.resize(PHYSFS_fileLength(myfile));
            std::int64_t length_read = PHYSFS_readBytes(myfile, buffer.data(), static_cast<std::uint64_t>(PHYSFS_fileLength(myfile)));
            assert(length_read == PHYSFS_fileLength(myfile));
            PHYSFS_close(myfile);

            std::vector<std::string> lines = split_lines(buffer, '\n');

            for (const auto& line : lines) {
                std::vector line_data = split_lines(line, ':');
                auto uuid_g = uuids::uuid::from_string(line_data.at(2));
                if (!uuid_g.has_value()) {
                    spdlog::error("Bad UUID in assets.lst: {}", line_data.at(2));
                    continue;
                }

                auto uuid = uuid_g.value();
                auto new_resource = std::make_unique<Resource>( line_data.at(1), uuid, static_cast<resource_type>(std::stoi(line_data.at(0))) );

                populate_resource(*new_resource);
                m_resources[uuid] = std::move(new_resource);
            }

        } else {
            // doesn't exists..
            throw Exception("Couldn't find asset manifest inside base.pak");
        }
    }

    auto ResourceManager::instance() -> ResourceManager& {
        static ResourceManager s_instance;
        return s_instance;
    }

    auto ResourceManager::getTexture(const uuids::uuid uuid) -> Texture& {
        ROSA_PROFILE_SCOPE("Assets:getTexture");

        if (auto search = m_resources.find(uuid); search != m_resources.end()) {
            assert(search->second->m_type == resource_texture);

            if (!search->second->m_loaded) {
                populate_resource(*(search->second));
            }

            return search->second->m_texture;
        }

        spdlog::error("Asset not registered {}", uuids::to_string(uuid));
        abort();
    }

    auto ResourceManager::getShader(const uuids::uuid uuid) -> Shader& {
        ROSA_PROFILE_SCOPE("Assets:getShader");

        if (auto search = m_resources.find(uuid); search != m_resources.end()) {
            assert(search->second->m_type == resource_vertex_shader ||
                    search->second->m_type == resource_fragment_shader);

            if (!search->second->m_loaded) {
                populate_resource(*(search->second));
            }

            return search->second->m_shader;
        }

        spdlog::error("Asset not registered {}", uuids::to_string(uuid));
        abort();
    }

    // auto ResourceManager::getFont(const uuids::uuid uuid) -> sf::Font& {
    //     ROSA_PROFILE_SCOPE("Assets:getFont");

    //     if (auto search = m_resources.find(uuid); search != m_resources.end()) {
    //         assert(search->second->m_type == resource_font);

    //         if (!search->second->m_loaded) {
    //             populate_resource(*(search->second));
    //         }

    //         return search->second->m_font;
    //     }

    //     spdlog::error("Asset not registered {}", uuids::to_string(uuid));
    //     abort();
    // }

    auto ResourceManager::getSound(const uuids::uuid uuid) -> AudioFile& {
        ROSA_PROFILE_SCOPE("Assets:getSound");

        if (auto search = m_resources.find(uuid); search != m_resources.end()) {
            assert(search->second->m_type == resource_sound);

            if (!search->second->m_loaded) {
                populate_resource(*(search->second));
            }

            return search->second->m_audio_file;
        }

        spdlog::error("Asset not registered {}", uuids::to_string(uuid));
        abort();
    }

    // auto ResourceManager::getMusicTrack(const uuids::uuid uuid) -> sf::Music& {
    //     ROSA_PROFILE_SCOPE("Assets:getMusicTrack");

    //     if (auto search = m_resources.find(uuid); search != m_resources.end()) {
    //         assert(search->second->m_type == resource_sound);

    //         if (!search->second->m_loaded) {
    //             populate_resource(*(search->second));
    //         }

    //         return search->second->m_music;
    //     }

    //     spdlog::error("Asset not registered {}", uuids::to_string(uuid));
    //     abort();
    // }

    auto ResourceManager::getScript(uuids::uuid uuid) -> const std::string& {
        ROSA_PROFILE_SCOPE("Assets:getScript");

        if (auto search = m_resources.find(uuid); search != m_resources.end()) {
            assert(search->second->m_type == resource_script);

            if (!search->second->m_loaded) {
                populate_resource(*(search->second));
            }

            return search->second->m_script;
        }

        spdlog::error("Asset not registered {}", uuids::to_string(uuid));
        abort();
    }

    // template<typename T>
    // auto ResourceManager::load_asset(const std::string& path) -> T {
    //     if (PHYSFS_exists(path.c_str()) != 0) {
    //         PhysFsStream stream;
    //         [[maybe_unused]] bool stream_success = stream.open(path.c_str());
    //         assert(stream_success);

    //         T asset{};
    //         [[maybe_unused]] bool load_success = asset.load(stream);
    //         assert(load_success);

    //         return asset;
    //     }
        
    //     spdlog::critical("Couldn't load asset: {}", path);
    //     return T{};
    // }

    auto ResourceManager::populate_resource(Resource& resource) -> bool {
        ROSA_PROFILE_SCOPE("Assets:Load");
        
        spdlog::debug("Loading asset {}", resource.m_name);
        auto result{false};

        switch (resource.m_type) {
            case resource_texture:
                resource.m_texture = Texture();
                resource.m_texture.loadFromPhysFS(resource.m_name);
                break;
            case resource_vertex_shader:
                resource.m_shader = Shader(VertexShader);
                resource.m_shader.loadFromPhysFS(resource.m_name);
                break;
            case resource_fragment_shader:
                resource.m_shader = Shader(FragmentShader);
                resource.m_shader.loadFromPhysFS(resource.m_name);
                break;
            // case resource_font:
            //     resource.m_font = load_asset<sf::Font>(resource.m_name);
            //     break;
            case resource_sound:
            case resource_music:
                result = resource.m_audio_file.loadFromPhysFS(resource.m_name);
                if (!result) {
                    spdlog::error("Failed to load sound {}", uuids::to_string(resource.m_uuid));
                }
                break;
            case resource_script:
                if (PHYSFS_exists(resource.m_name.c_str()) != 0) {
                    PHYSFS_file* myfile = PHYSFS_openRead(resource.m_name.c_str());
                    std::string buffer{};
                    buffer.resize(PHYSFS_fileLength(myfile));
                    std::int64_t length_read = PHYSFS_readBytes(myfile, buffer.data(), static_cast<std::uint64_t>(PHYSFS_fileLength(myfile)));
                    assert(length_read == PHYSFS_fileLength(myfile));
                    PHYSFS_close(myfile);

                    resource.m_script = buffer;
                } else {
                    spdlog::critical("Couldn't load script: {}", uuids::to_string(resource.m_uuid));
                }
                break;
            case resource_font:
                break;
        }

        resource.m_loaded = true;
        return true;
    }

} // namespace rosa