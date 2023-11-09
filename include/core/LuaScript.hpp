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

#include <core/Resource.hpp>
#include <string>
#include <physfs.h>

namespace rosa {

    class LuaScript : public ::rosa::Resource {
        public:
            LuaScript(std::string name, uuids::uuid uuid) : rosa::Resource(std::move(name), uuid) {}

            auto loadFromPhysFS() -> bool override;

            auto getContent() const -> const std::string&;

        private:
            std::string m_content;
    };

} // namespace rosa