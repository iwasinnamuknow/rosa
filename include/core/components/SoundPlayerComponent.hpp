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
 *  You should have received a copy of the GNU General Public License along with bbai. If not,
 *  see <https://www.gnu.org/licenses/>.
 */

#pragma once

#include <core/ResourceManager.hpp>
#include <audio/AudioManager.hpp>
#include <spdlog/spdlog.h>
#include <stduuid/uuid.h>
#include <soloud/include/soloud_wav.h>

namespace rosa {

    struct SoundPlayerComponent {

        auto getAudio() const -> uuids::uuid {
            return m_uuid;
        }

        auto setAudio(uuids::uuid uuid) -> void {
            m_uuid = uuid;
            m_audio_file = &ResourceManager::instance().getSound(m_uuid);
            auto result = m_wav.loadFile(m_audio_file);
            if (result != SoLoud::SO_NO_ERROR) {
                spdlog::error("Failed to load audio file {}", uuids::to_string(m_uuid));
            }
        }

        auto play() -> void { 
            m_handle = AudioManager::instance().play(m_wav);
        }

        auto stop() -> void { 
            AudioManager::instance().stop(m_handle);
        }

        auto setVolume(float volume) {
            m_volume = volume;
            m_wav.setVolume(m_volume);
        }

        auto getVolume() -> float {
            return m_volume;
        }

        private:
            SoLoud::Wav m_wav;
            uuids::uuid m_uuid;

            bool m_playing{false};
            float m_volume{1.0F};

            AudioFile* m_audio_file{nullptr};

            unsigned int m_handle;
    };

} // namespace rosa