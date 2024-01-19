/*
 * This file is part of rosa.
 *
 *  glad is free software: you can redistribute it and/or modify it under the terms of the
 *  GNU General Public License as published by the Free Software Foundation, either version
 *  3 of the License, or (at your option) any later version.
 *
 *  glad is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without
 *  even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along with rosa. If not,
 *  see <https://www.gnu.org/licenses/>.
 */

#pragma once

#include <cstdint>

namespace rosa {

    /**
     * \brief Simple colour representation
     *
     * All values range from 0.0 to 1.0
     */
    struct Colour {

        /**
         * \brief Initialise a default colour
         */
        Colour() = default;

        /**
         * \brief Initialise a colour with three channels
         * \param red red value
         * \param green green value
         * \param blue blue value
         */
        Colour(float red, float green, float blue) : r(red), g(green), b(blue) {}

        /**
         * \brief Initialise a colour with four channels
         * \param red red value
         * \param green green value
         * \param blue blue value
         * \param alpha alpha value
         */
        Colour(float red, float green, float blue, float alpha) : r(red), g(green), b(blue), a(alpha) {}

        float r{1}; /**< red value   */
        float g{1}; /**< green value */
        float b{1}; /**< blue value  */
        float a{1}; /**< alpha value */

    };

} // namespace rosa
