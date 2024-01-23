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

#include <core/Exception.hpp>
#include <lodepng.h>
#include <vector>

namespace rosa {

    /**
     * \brief Used to compare graphics output with reference images
     *
     * Currently only supports very simple pixel-for-pixel equality test
     */
    class ImageComparator {
    public:
        /**
         * \brief Compare pixels of two images
         *
         * This compares pixels directly with each other with no intelligence or finesse.
         *
         * \param lhs image data
         * \param rhs other image data
         * \return true if all pixels match, false otherwise
         */
        static auto compareEqualityBasic(std::span<const unsigned char> lhs, std::span<const unsigned char> rhs) -> bool;

        /**
         * \brief Write image data to a png file
         * \param filename destination filename on disk
         * \param image span of image data 32bpp RGBA
         * \param width image width
         * \param height image height
         */
        static auto writePNG(const std::string& filename, std::span<const unsigned char> image, unsigned width, unsigned height) -> void;

        /**
         * \brief Load image data from an on-disk png file
         * \param filename source filename on disk
         * \return A vector of image data 32bpp RGBA
         */
        static auto readPNG(const std::string& filename) -> std::vector<unsigned char>;

        /**
         * \brief Write the difference of two images to a png file
         * \param filename destination filename on disk
         * \param image_a span of image A data 32bpp RGBA
         * \param image_b span of image B data 32bpp RGBA
         * \param width image width
         * \param height image height
         */
        static auto writePNGDifferential(
                const std::string&             filename,
                std::span<const unsigned char> image_a,
                std::span<const unsigned char> image_b,
                unsigned                       width,
                unsigned                       height) -> void;
    };

}// namespace rosa