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

#include <audio/AudioFile.hpp>
#include <core/components/SoundPlayerComponent.hpp>

namespace rosa {

    auto SoundPlayerComponent::getAudioUuid() const -> const Uuid& {
        return m_uuid;
    }

    auto SoundPlayerComponent::setAudio(const Uuid& uuid) -> void {
        m_uuid = uuid;
        m_audio_file = &ResourceManager::getInstance().getAsset<AudioFile>(m_uuid);
        auto result = m_wav.loadFile(m_audio_file);
        if (result != SoLoud::SO_NO_ERROR) {
            spdlog::error("Failed to load audio file {}", static_cast<std::string>(m_uuid));
        }
    }

    auto SoundPlayerComponent::play() -> void { 
        m_handle = AudioManager::instance().play(m_wav);
    }

    auto SoundPlayerComponent::stop() -> void { 
        AudioManager::instance().stop(m_handle);
    }

    auto SoundPlayerComponent::setDefaultVolume(float volume) -> void {
        m_default_volume = volume;
        m_wav.setVolume(volume);
    }

    auto SoundPlayerComponent::getLength() -> double {
        return m_wav.getLength();
    }

    auto SoundPlayerComponent::setLooping(bool loop) -> void {
        m_wav.setLooping(loop);
    }

    auto SoundPlayerComponent::setVolume(float volume) -> void {
        m_volume = volume;
        AudioManager::instance().setVoiceVolume(m_handle, volume);
    }

    auto SoundPlayerComponent::getPosition() const -> double {
        return AudioManager::instance().getVoicePosition(m_handle);
    }

    auto SoundPlayerComponent::setPosition(double position) -> void {
        AudioManager::instance().setVoicePosition(m_handle, position);
    }

    auto SoundPlayerComponent::setPause(bool pause) -> void {
        AudioManager::instance().setVoicePause(m_handle, pause);
    }

    auto SoundPlayerComponent::getPause() const -> bool {
        return AudioManager::instance().getVoicePause(m_handle);
    }

    auto SoundPlayerComponent::isPlaying() const -> bool {
        return AudioManager::instance().checkHandle(m_handle);
    }

    auto SoundPlayerComponent::getVolume() const -> float {
        return m_volume;
    }

    auto SoundPlayerComponent::getDefaultVolume() const -> float {
        return m_default_volume;
    }

    auto operator<<(YAML::Emitter& out, const SoundPlayerComponent& component) -> YAML::Emitter& {
        out << YAML::BeginMap;
        out << YAML::Key << "type" << YAML::Value << "sound";
        out << YAML::Key << "source" << YAML::Value << static_cast<std::string>(component.getAudioUuid());
        out << YAML::Key << "position" << YAML::Value << component.getPosition();
        out << YAML::Key << "volume" << YAML::Value << component.getVolume();
        out << YAML::Key << "default_volume" << YAML::Value << component.getDefaultVolume();
        out << YAML::Key << "paused" << YAML::Value << component.getPause();
        out << YAML::Key << "playing" << YAML::Value << component.isPlaying();
        out << YAML::EndMap;
        return out;
    }

} // namespace rosa