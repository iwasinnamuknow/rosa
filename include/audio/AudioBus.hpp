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

#include <soloud/include/soloud_bus.h>

namespace rosa {

    /**
     * \brief An audio bus that can be used to route various audio streams
     *
     * Allows audio to be grouped together for volume fading etc.
     */
    class AudioBus : public SoLoud::Bus {
        public:
        unsigned int handle{0}; /**< Bus handle */
        float volume{1.F};      /**< Bus volume cached here as we can't retrieve it from the engine */
    };

} // namespace rosa