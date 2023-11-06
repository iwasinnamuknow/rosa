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

    auto AudioManager::setVoiceVolume(unsigned int handle, float volume) -> void {
        m_engine->setVolume(handle, volume);
    }

    auto AudioManager::getVoiceVolume(unsigned int handle) -> float {
        return m_engine->getVolume(handle);
    }

    auto AudioManager::setGlobalVolumne(float volume) -> void {
        m_engine->setGlobalVolume(volume);
    }

    auto AudioManager::createBus(std::string name) -> void {
        AudioBus bus;
        SoLoud::Bus so_bus;
        bus.handle = m_engine->play(so_bus);
        m_busses.insert({std::move(name), std::move(bus)});
    }

    auto AudioManager::removeBus(const std::string& name) -> void {
        m_engine->stop(m_busses.at(name).handle);
        m_busses.erase(name);
    }

    auto AudioManager::playOnBus(SoLoud::AudioSource& source, std::string bus_name) -> unsigned int{
        return m_busses.at(bus_name).play(source);
    }

    auto AudioManager::setBusVolume(float volume, const std::string& bus_name) -> void {
        m_busses.at(bus_name).setVolume(volume);
    }

    auto AudioManager::getVoicePosition(unsigned int handle) -> double {
        return m_engine->getStreamPosition(handle);
    }

    auto AudioManager::setVoicePosition(unsigned int handle, double position) -> void {
        m_engine->seek(handle, position);
    }

    auto AudioManager::setVoicePause(unsigned int handle, bool pause) -> void {
        m_engine->setPause(handle, pause);
    }

    auto AudioManager::getVoicePause(unsigned int handle) -> bool {
        return m_engine->getPause(handle);
    }

    auto AudioManager::checkHandle(unsigned int handle) -> bool {
        return m_engine->isValidVoiceHandle(handle);
    }

} // namespace rosa