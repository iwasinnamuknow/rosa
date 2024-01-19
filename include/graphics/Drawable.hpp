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

#include <graphics/RenderWindow.hpp>
#include <graphics/Vertex.hpp>
#include <graphics/Quad.hpp>
#include <graphics/Shader.hpp>
#include <glm/glm.hpp>

namespace rosa {

    /**
     * \brief Base class that all drawable types inherit from
     */
    class Drawable {
    public:
        /**
         * \brief Default constructor
         */
        Drawable() = default;

        /**
         * \brief Copy constructor
         */
        Drawable(const Drawable &) = default;

        /**
         * \brief Move constructor
         */
        Drawable(Drawable &&) = default;

        /**
         * \brief Copy assignment
         * \return a copy of the drawable
         */
        auto operator=(const Drawable &) -> Drawable & = default;

        /**
         * \brief Move assignment
         * \return the moved drawable
         */
        auto operator=(Drawable &&) -> Drawable & = default;

        /**
         * \brief Virtual default destructor
         */
        virtual ~Drawable() = default;

        /**
         * \brief Virtual function for draw operations
         * \param transform the transform of the drawable
         */
        virtual auto draw(glm::mat4 /*transform*/) -> void {}

    protected:
        friend class RenderWindow;
    };

} // namespace rosa