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

namespace rosa {

    ResourceManager::ResourceManager() {
        ROSA_PROFILE_SCOPE("Assets:ParseAssetList");

        PHYSFS_init(nullptr);
        int mount_success = PHYSFS_mount(get_exe_dir().append("/base.pak").data(), "", 0);
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
                auto uuid = Uuid(line_data.at(2));
                auto type = static_cast<resource_type>(std::stoi(line_data.at(0)));

                if (type == resource_type::resource_texture) {
                    auto new_resource = std::make_unique<Texture>( line_data.at(1), uuid);
                    new_resource->loadFromPhysFS();
                    m_resources[uuid] = std::move(new_resource);
                
                } else if (type == resource_type::resource_vertex_shader) {
                    auto new_resource = std::make_unique<Shader>( line_data.at(1), uuid, ShaderType::VertexShader);
                    new_resource->loadFromPhysFS();
                    m_resources[uuid] = std::move(new_resource);

                } else if (type == resource_type::resource_fragment_shader) {
                    auto new_resource = std::make_unique<Shader>( line_data.at(1), uuid, ShaderType::FragmentShader);
                    new_resource->loadFromPhysFS();
                    m_resources[uuid] = std::move(new_resource);

                } else if (type == resource_type::resource_script) {
                    auto new_resource = std::make_unique<LuaScript>( line_data.at(1), uuid);
                    new_resource->loadFromPhysFS();
                    m_resources[uuid] = std::move(new_resource);

                } else if (type == resource_type::resource_sound || type == resource_type::resource_music) {
                    auto new_resource = std::make_unique<AudioFile>( line_data.at(1), uuid);
                    new_resource->loadFromPhysFS();
                    m_resources[uuid] = std::move(new_resource);
                }     
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

} // namespace rosa