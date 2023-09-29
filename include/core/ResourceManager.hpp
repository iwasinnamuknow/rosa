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

#pragma once

#include <memory>
#include <unordered_map>
#include <vector>

namespace rosa {

    class ResourceManager {
        public:
            explicit ResourceManager() = default;
            ResourceManager(ResourceManager const &) = delete;
            auto operator=(ResourceManager const &) -> ResourceManager & = delete;
            ResourceManager(ResourceManager const &&) = delete;
            auto operator=(ResourceManager const &&) -> ResourceManager & = delete;
            ~ResourceManager() = delete;
    };

} // namespace rosa