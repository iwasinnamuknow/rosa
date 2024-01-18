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

    class AudioManager {
        public:
            static auto instance() -> AudioManager&;

            auto play(SoLoud::AudioSource& source) -> unsigned int;
            auto stop(unsigned int handle) -> void;

            auto setVoiceVolume(unsigned int handle, float volume) -> void;
            auto getVoiceVolume(unsigned int handle) -> float;

            auto setGlobalVolumne(float volume) -> void;

            auto createBus(std::string name) -> void;
            auto removeBus(const std::string& name) -> void;

            auto playOnBus(SoLoud::AudioSource& source, std::string bus_name) -> unsigned int;
            auto setBusVolume(float volume, const std::string& bus_name) -> void;

            auto getVoicePosition(unsigned int handle) -> double;
            auto setVoicePosition(unsigned int handle, double position) -> void;

            auto setVoicePause(unsigned int handle, bool pause) -> void;
            auto getVoicePause(unsigned int handle) -> bool;

            auto checkHandle(unsigned int handle) -> bool;
        private:
            AudioManager();
            ~AudioManager();

            std::unique_ptr<SoLoud::Soloud> m_engine;

            std::unordered_map<std::string, AudioBus> m_busses;
    };

} // namespace rosa