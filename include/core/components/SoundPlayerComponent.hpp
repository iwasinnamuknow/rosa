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

#include <core/Uuid.hpp>
#include <core/ResourceManager.hpp>
#include <audio/AudioManager.hpp>

#include <spdlog/spdlog.h>
#include <soloud/include/soloud_wav.h>

namespace rosa {

    struct SoundPlayerComponent {

        auto getAudio() const -> Uuid;
        auto setAudio(Uuid uuid) -> void;

        auto play() -> void;
        auto stop() -> void;

        auto setDefaultVolume(float volume) -> void;

        auto getLength() -> double;

        auto setLooping(bool loop) -> void;

        auto setVolume(float volume) -> void;

        auto getPosition() const -> double;
        auto setPosition(double position) -> void;

        auto setPause(bool pause) -> void;
        auto getPause() const -> bool;

        auto isPlaying() const -> bool;

        private:
            SoLoud::Wav m_wav;
            Uuid m_uuid;

            float m_volume{1.F};
            float m_default_volume{1.F};

            AudioFile* m_audio_file{nullptr};
            unsigned int m_handle;

            friend class SceneSerialiser;
    };

} // namespace rosa