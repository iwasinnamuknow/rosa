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

#include <SFML/Audio/Music.hpp>
#include <SFML/Audio/Sound.hpp>
#include <SFML/Graphics/Shader.hpp>
#include <cassert>
#include <core/ResourceManager.hpp>

namespace rosa {

    ResourceManager::ResourceManager() {
        [[maybe_unused]] int mount_success = PHYSFS_mount("base.pak", "", 1);
        assert(mount_success != 0);
    }

    auto ResourceManager::instance() -> ResourceManager& {
        static ResourceManager s_instance;
        return s_instance;
    }

    auto ResourceManager::getTexture(const std::string &path) -> sf::Texture& {
        if (auto search = m_textures.find(path); search != m_textures.end()) {
            return search->second;
        }

        PhysFsStream stream;
        [[maybe_unused]] bool stream_success = stream.open(path.c_str());
        assert(stream_success);

        m_textures[path] = sf::Texture{};
        [[maybe_unused]] bool load_success = m_textures[path].loadFromStream(stream);
        assert(load_success);

        return m_textures[path];
    }

    auto ResourceManager::getFont(const std::string &path) -> sf::Font& {
        if (auto search = m_fonts.find(path); search != m_fonts.end()) {
            return search->second;
        }

        PhysFsStream stream;
        [[maybe_unused]] bool stream_success = stream.open(path.c_str());
        assert(stream_success);

        m_fonts[path] = sf::Font{};
        [[maybe_unused]] bool load_success = m_fonts[path].loadFromStream(stream);
        assert(load_success);

        return m_fonts[path];
    }

    auto ResourceManager::getSound(const std::string &path) -> sf::SoundBuffer& {
        if (auto search = m_sounds.find(path); search != m_sounds.end()) {
            return search->second;
        }

        PhysFsStream stream;
        [[maybe_unused]] bool stream_success = stream.open(path.c_str());
        assert(stream_success);

        m_sounds[path] = sf::SoundBuffer{};
        [[maybe_unused]] bool load_success = m_sounds[path].loadFromStream(stream);
        assert(load_success);

        return m_sounds[path];
    }

    auto ResourceManager::getMusicTrack(const std::string &path) -> sf::Music& {
        if (auto search = m_music_tracks.find(path); search != m_music_tracks.end()) {
            return search->second;
        }

        PhysFsStream stream;
        [[maybe_unused]] bool stream_success = stream.open(path.c_str());
        assert(stream_success);

        //m_music_tracks[path] = sf::Music{};
        [[maybe_unused]] bool load_success = m_music_tracks[path].openFromStream(stream);
        assert(load_success);

        return m_music_tracks[path];
    }

} // namespace rosa