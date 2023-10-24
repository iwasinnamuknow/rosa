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

#include <core/Resource.hpp>
#include <cassert>
#include <core/ResourceManager.hpp>
#include <exception>
#include <memory>
#include <optional>
#include <spdlog/spdlog.h>
#include <sstream>
#include <string>
#include <utility>

#define ROSA_PRELOAD_ASSETS

namespace rosa {

    ResourceManager::ResourceManager() {
        ROSA_PROFILE_SCOPE("Assets:ParseAssetList");

        [[maybe_unused]] int mount_success = PHYSFS_mount("base.pak", "", 1);
        assert(mount_success != 0);

        // Attempt to read assets.lst
        if (PHYSFS_exists("assets.lst") != 0) {
            PHYSFS_file* myfile = PHYSFS_openRead("assets.lst");
            char *buffer = new char[PHYSFS_fileLength(myfile)];
            int length_read = PHYSFS_readBytes (myfile, buffer, PHYSFS_fileLength(myfile));
            assert(length_read == PHYSFS_fileLength(myfile));
            PHYSFS_close(myfile);

            std::string string_data(buffer, static_cast<size_t>(length_read));
            std::vector<std::string> lines = split_lines(string_data, '\n');

            for (const auto& line : lines) {
                std::vector line_data = split_lines(line, ':');
                auto uuid = uuids::uuid::from_string(line_data.at(2)).value();
                auto new_resource = std::make_unique<Resource>( line_data.at(1), uuid, static_cast<resource_type>(std::stoi(line_data.at(0))) );

#ifdef ROSA_PRELOAD_ASSETS
                populate_resource(*new_resource);
#endif // ROSA_PRELOAD_ASSETS
                m_resources[uuid] = std::move(new_resource);
            }

        } else {
            // doesn't exists..
            spdlog::critical("Couldn't find asset registry inside base base.pak");
            abort();
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

    // auto ResourceManager::getSound(const uuids::uuid uuid) -> sf::SoundBuffer& {
    //     ROSA_PROFILE_SCOPE("Assets:getSound");

    //     if (auto search = m_resources.find(uuid); search != m_resources.end()) {
    //         assert(search->second->m_type == resource_sound);

    //         if (!search->second->m_loaded) {
    //             populate_resource(*(search->second));
    //         }

    //         return search->second->m_sound_buffer;
    //     }

    //     spdlog::error("Asset not registered {}", uuids::to_string(uuid));
    //     abort();
    // }

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

    auto ResourceManager::split_lines(const std::string& string, char delimiter) -> std::vector<std::string> {
        auto result = std::vector<std::string>{};
        auto stream = std::stringstream(string);

        for (std::string line; std::getline(stream, line, delimiter);) {
            result.push_back(line);
        }

        return result;
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
            // case resource_font:
            //     resource.m_font = load_asset<sf::Font>(resource.m_name);
            //     break;
            // case resource_sound:
            //     resource.m_sound_buffer = load_asset<sf::SoundBuffer>(resource.m_name);
            //     break;
            // case resource_music:
            //     if (PHYSFS_exists(resource.m_name.c_str()) != 0) {
            //         [[maybe_unused]] bool stream_success = resource.m_stream.open(resource.m_name.c_str());
            //         assert(stream_success);

            //         resource.m_music.openFromStream(resource.m_stream);
            //     } else {
            //         spdlog::critical("Couldn't load asset: {}", resource.m_name);
            //     }
            //     break;
            case resource_script:
                if (PHYSFS_exists(resource.m_name.c_str()) != 0) {
                    PHYSFS_file* myfile = PHYSFS_openRead(resource.m_name.c_str());
                    char *buffer = new char[PHYSFS_fileLength(myfile)];
                    int length_read = PHYSFS_readBytes (myfile, buffer, PHYSFS_fileLength(myfile));
                    assert(length_read == PHYSFS_fileLength(myfile));
                    PHYSFS_close(myfile);

                    std::string string_data(buffer, static_cast<size_t>(length_read));

                    resource.m_script = string_data;
                } else {
                    spdlog::critical("Couldn't load asset: {}", resource.m_name);
                }
                break;
        }

        resource.m_loaded = true;
        return true;
    }

} // namespace rosa