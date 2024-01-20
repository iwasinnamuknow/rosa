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

#include <vector>
#include <lodepng.h>
#include <core/Exception.hpp>

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
        static auto compareEqualityBasic(std::span<const unsigned char> lhs, std::span<unsigned char> rhs) -> bool {

            // Images must be of equal size
            if (lhs.size() != rhs.size()) {
                return false;
            }

            auto size = lhs.size();

            // Compare each pixel, very simple
            for (
                    std::size_t index = 0; index < size; index++
                    ) {
                if (lhs[index] != rhs[index]) {
                    return false;
                }
            }

            return true;
        }

        /**
         * \brief Write image data to a png file
         * \param filename destination filename on disk
         * \param image span of image data 32bpp RGBA
         * \param width image width
         * \param height image height
         */
        static auto writePNG(const std::string& filename, std::span<unsigned char> image, unsigned width, unsigned height) -> void {
            //Encode the image
            unsigned error = lodepng::encode(filename, image.data(), width, height);

            //if there's an error, display it
            if (error != 0) {
                throw Exception(fmt::format("PNG encoder error in {}: {} ({})", filename, error, lodepng_error_text(error)));
            }
        }

        /**
         * \brief Load image data from an on-disk png file
         * \param filename source filename on disk
         * \return A vector of image data 32bpp RGBA
         */
        static auto readPNG(const std::string& filename) -> std::vector<unsigned char> {
            std::vector<unsigned char> image; //the raw pixels
            unsigned width{0};
            unsigned height{0};

            //decode
            unsigned error = lodepng::decode(image, width, height, filename);

            if (error != 0) {
                throw Exception(fmt::format("PNG decoder error in {}: {} ({})", filename, error, lodepng_error_text(error)));
            }

            //the pixels are now in the vector "image", 4 bytes per pixel, ordered RGBARGBA..., use it as texture, draw it, ...
            return image;
        }

    };

} // namespace rosa