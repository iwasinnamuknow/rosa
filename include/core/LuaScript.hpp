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

#include <core/Resource.hpp>
#include <core/Uuid.hpp>
#include <physfs.h>
#include <string>

namespace rosa {

    /**
     * \brief A single script asset
     *
     * This Resource type allows a Lua script to be loaded from assets. Very simply it holds the text
     * of a script and supplies it when required for loading.
     */
    class LuaScript : public ::rosa::Resource {
    public:
        LuaScript(const std::string& name, const Uuid& uuid, const std::string& pack);

        /**
         * \brief Get the script text
         */
        auto getContent() const -> const std::string&;

    private:
        std::string m_content;
    };

}// namespace rosa