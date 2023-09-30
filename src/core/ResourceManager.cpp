/*
 * This file is part of rosa.
 *
 *  rosa is free software: you can redistribute it and/or modify it under the
 * terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later
 * version.
 *
 *  rosa is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
 * A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along with
 * bbai. If not, see <https://www.gnu.org/licenses/>.
 */

#include <cassert>
#include <core/ResourceManager.hpp>

namespace rosa {

    auto ResourceManager::getTexture(const std::string &path) -> sf::Texture& {
        if (auto search = m_textures.find(path); search != m_textures.end()) {
            return search->second;
        }

        m_textures[path] = sf::Texture{};
        assert(m_textures[path].loadFromFile(path));
        return m_textures[path];
    }

} // namespace rosa