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

#include <core/components/MusicPlayerComponent.hpp>

namespace rosa {

    auto MusicPlayerComponent::getAudioUuid() const -> const Uuid& {
        return m_uuid;
    }

    auto MusicPlayerComponent::setAudio(const Uuid& uuid) -> void {
        m_uuid = uuid;
        m_audio_file = &ResourceManager::getInstance().getAsset<AudioFile>(m_uuid);
        auto result = m_wav_stream.loadFile(m_audio_file);
        if (result != SoLoud::SO_NO_ERROR) {
            spdlog::error("Failed to load audio file {}", static_cast<std::string>(m_uuid));
        }
    }

    auto MusicPlayerComponent::play() -> void { 
        m_handle = AudioManager::instance().play(m_wav_stream);
    }

    auto MusicPlayerComponent::stop() -> void { 
        AudioManager::instance().stop(m_handle);
    }

    auto MusicPlayerComponent::setDefaultVolume(float volume) -> void {
        m_wav_stream.setVolume(volume);
    }

    auto MusicPlayerComponent::getLength() -> double {
        return m_wav_stream.getLength();
    }

    auto MusicPlayerComponent::setLooping(bool loop) -> void {
        m_wav_stream.setLooping(loop);
    }

    auto MusicPlayerComponent::setVolume(float volume) -> void {
        AudioManager::instance().setVoiceVolume(m_handle, volume);
    }

    auto MusicPlayerComponent::getPosition() const -> double {
        return AudioManager::instance().getVoicePosition(m_handle);
    }

    auto MusicPlayerComponent::setPosition(double position) -> void {
        AudioManager::instance().setVoicePosition(m_handle, position);
    }

    auto MusicPlayerComponent::setPause(bool pause) -> void {
        AudioManager::instance().setVoicePause(m_handle, pause);
    }

    auto MusicPlayerComponent::getPause() const -> bool {
        return AudioManager::instance().getVoicePause(m_handle);
    }

    auto MusicPlayerComponent::isPlaying() const -> bool {
        return AudioManager::instance().checkHandle(m_handle);
    }

    auto MusicPlayerComponent::getDefaultVolume() const -> float {
        return m_default_volume;
    }

    auto MusicPlayerComponent::getVolume() const -> float {
        return m_volume;
    }

} // namespace rosa