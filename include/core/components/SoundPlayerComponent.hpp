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

#pragma once

#include <core/Uuid.hpp>
#include <core/ResourceManager.hpp>
#include <audio/AudioManager.hpp>

#include <spdlog/spdlog.h>
#include <soloud/include/soloud_wav.h>

namespace rosa {

    /**
     * \brief A component for playing audio from memory
     *
     * This component is best used for short sounds effects, if you are playing
     * long tracks, it maybe be better to stream the audio with a MusicPlayerComponent.
     */
    struct SoundPlayerComponent {

        /**
         * \brief Get the Uuid of the associated asset
         */
        auto getAudio() const -> Uuid;

        /**
         * \brief Load an asset by Uuid
         */
        auto setAudio(Uuid uuid) -> void;

        /**
         * \brief Play the audio track
         */
        auto play() -> void;

        /**
         * \brief Stop the audio track
         */
        auto stop() -> void;

        /**
         * \brief Set the default volume
         */
        auto setDefaultVolume(float volume) -> void;

        /**
         * \brief Get the default volume
         */
        auto getDefaultVolume() const -> float;

        /**
         * \brief Get the length of the audio track in seconds
         */
        auto getLength() -> double;

        /**
         * \brief Set whether to loop the audio track
         */
        auto setLooping(bool loop) -> void;

        /**
         * \brief Set the volume of a playing track
         */
        auto setVolume(float volume) -> void;

        /**
         * \brief Get the volume of a playing track
         */
        auto getVolume() const -> float;

        /**
         * \brief Get the position of the playhead in seconds
         */
        auto getPosition() const -> double;

        /**
         * \brief Set the position of the playhead in seconds
         */
        auto setPosition(double position) -> void;

        /**
         * \brief Pause or unpause a playing track
         */
        auto setPause(bool pause) -> void;

        /**
         * \brief Get the pause state of a playing track
         */
        auto getPause() const -> bool;

        /**
         * \brief Determine if a track is currently playing
         */
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