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

#include <memory>
#include <soloud/include/soloud.h>

namespace rosa {

    class AudioManager {
        public:
            static auto instance() -> AudioManager&;

            auto play(SoLoud::AudioSource& source) -> unsigned int;
            auto stop(unsigned int handle) -> void;

            auto setVoiceVolume(unsigned int handle, float volume) -> void;
            auto setGlobalVolumne(float volume) -> void;
        private:
            AudioManager();
            ~AudioManager();

            std::unique_ptr<SoLoud::Soloud> m_engine;
    };

} // namespace rosa