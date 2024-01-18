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

#include <memory>
#include <soloud/include/soloud.h>
#include <soloud/include/soloud_audiosource.h>
#include <string>
#include <unordered_map>
#include <audio/AudioBus.hpp>

namespace rosa {

    /**
     * \brief Global manager for all audio playback
     */
    class AudioManager {
    public:
        /**
         * \brief Get the global instance
         */
        static auto instance() -> AudioManager &;

        /**
         * \brief Play an audio source
         * \param source audio data
         * \return a handle for further operations
         */
        auto play(SoLoud::AudioSource &source) -> unsigned int;

        /**
         * \brief Stop playing an audio source
         * \param handle the handle provided by play()
         */
        auto stop(unsigned int handle) -> void;

        /**
         * \brief Set the volume of a particular voice
         * \param handle the handle provided by play()
         * \param volume the volume to set from 0.0 to 1.0
         */
        auto setVoiceVolume(unsigned int handle, float volume) -> void;

        /**
         * \brief Get the volume of a particular voice
         * \param handle the handle provided by play()
         * \return the volume of the voice
         */
        auto getVoiceVolume(unsigned int handle) const -> float;

        /**
         * \brief Set the global volume
         * \param volume the volume to set from 0.0 to 1.0
         */
        auto setGlobalVolume(float volume) -> void;

        /**
         * \brief Get the global volume
         * \return the global volume
         */
        auto getGlobalVolume() const -> float;

        /**
         * \brief Create a bus to route voices
         * \param name a string name for future access
         */
        auto createBus(std::string name) -> void;

        /**
         * \brief Destroy a bus
         * \param name string name
         */
        auto removeBus(const std::string &name) -> void;

        /**
         * \brief Play an audio source on a bus
         * \param source audio data
         * \param bus_name the string name of a created bus
         * \return a handle for further operations
         */
        auto playOnBus(SoLoud::AudioSource &source, std::string bus_name) -> unsigned int;

        /**
         * \brief Set the volume of a bus
         * \param volume the volume to set from 0.0 to 1.0
         * \param bus_name the string name of a created bus
         */
        auto setBusVolume(float volume, const std::string &bus_name) -> void;

        /**
         * \brief Get the volume of a bus
         * \param bus_name the string name of a created bus
         * \return the bus volume
         */
        auto getBusVolume(const std::string &bus_name) -> float;

        /**
         * \brief Get the position of the playhead for a voice
         * \param handle the handle provided by play()
         * \return the position of the playhead in seconds
         */
        auto getVoicePosition(unsigned int handle) -> double;

        /**
         * \brief Set the position of the playhead for a voice
         * \param handle the handle provided by play()
         * \param position the position of the playhead in seconds
         */
        auto setVoicePosition(unsigned int handle, double position) -> void;

        /**
         * \brief Set the pause state of a playing voice
         * \param handle the handle provided by play()
         * \param pause pause boolean
         */
        auto setVoicePause(unsigned int handle, bool pause) -> void;

        /**
         * \brief Get the pause state of a playing voice
         * \param handle the handle provided by play()
         * \return paused or unpaused
         */
        auto getVoicePause(unsigned int handle) -> bool;

        /**
         * \brief Check if the specified handle is a valid voice
         * \param handle the handle provided by play()
         * \return valid or invalid
         */
        auto checkHandle(unsigned int handle) -> bool;
    private:
        AudioManager();
        ~AudioManager();

        std::unique_ptr<SoLoud::Soloud> m_engine;

        std::unordered_map<std::string, AudioBus> m_busses;
    };

} // namespace rosa