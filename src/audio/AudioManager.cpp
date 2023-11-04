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

#include <audio/AudioManager.hpp>
#include <memory>
#include <soloud/include/soloud.h>

namespace rosa {

    AudioManager::AudioManager() : m_engine(std::make_unique<SoLoud::Soloud>()) {
        m_engine->init();
    }

    AudioManager::~AudioManager() {
        m_engine->deinit();
    }

    auto AudioManager::instance() -> AudioManager& {
        static AudioManager s_instance;
        return s_instance;
    }

    auto AudioManager::play(SoLoud::AudioSource& source) -> unsigned int {
        return m_engine->play(source);
    }

    auto AudioManager::stop(unsigned int handle) -> void {
        m_engine->stop(handle);
    }

} // namespace rosa