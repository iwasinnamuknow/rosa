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

#include <graphics/Colour.hpp>
#include <graphics/ImageComparator.hpp>
#include <span>

namespace rosa {
    auto ImageComparator::compareEqualityBasic(std::span<const unsigned char> lhs, std::span<const unsigned char> rhs) -> bool {

        // Images must be of equal size
        if (lhs.size() != rhs.size()) {
            return false;
        }

        auto size = lhs.size();

        // Compare each pixel, very simple
        for (std::size_t index = 0; index < size; index++) {
            if (lhs[index] != rhs[index]) {
                return false;
            }
        }

        return true;
    }

    auto ImageComparator::writePNG(const std::string& filename, std::span<const unsigned char> image, unsigned width, unsigned height) -> void {
        //Encode the image
        unsigned error = lodepng::encode(filename, image.data(), width, height);

        //if there's an error, display it
        if (error != 0) {
            throw Exception(fmt::format("PNG encoder error in {}: {} ({})", filename, error, lodepng_error_text(error)));
        }
    }

    auto ImageComparator::readPNG(const std::string& filename) -> std::vector<unsigned char> {
        std::vector<unsigned char> image;
        unsigned                   width{0};
        unsigned                   height{0};

        unsigned error = lodepng::decode(image, width, height, filename);

        if (error != 0) {
            throw Exception(fmt::format("PNG decoder error in {}: {} ({})", filename, error, lodepng_error_text(error)));
        }

        // 4 bytes per pixel, ordered RGBARGBA
        return image;
    }

    auto ImageComparator::writePNGDifferential(
            const std::string&             filename,
            std::span<const unsigned char> image_a,
            std::span<const unsigned char> image_b,
            unsigned                       width,
            unsigned                       height) -> void {

        std::vector<unsigned char> result{};
        result.reserve(image_a.size());

        int         pixels_different{0};
        std::size_t index{0};

        while (index < image_a.size()) {
            auto colour_a = Colour(
                    static_cast<float>(image_a[index] / 254.99),
                    static_cast<float>(image_a[index + 1] / 254.99),
                    static_cast<float>(image_a[index + 2] / 254.99),
                    static_cast<float>(image_a[index + 3] / 254.99));

            auto colour_b = Colour(
                    static_cast<float>(image_b[index] / 254.99),
                    static_cast<float>(image_b[index + 1] / 254.99),
                    static_cast<float>(image_b[index + 2] / 254.99),
                    static_cast<float>(image_b[index + 3] / 254.99));

            auto difference = colour_a - colour_b;
            if (difference.zero()) {
                pixels_different++;
            }

            result.push_back(static_cast<std::uint8_t>(image_a[index]) - static_cast<std::uint8_t>(image_b[index]));
            result.push_back(static_cast<std::uint8_t>(image_a[index + 1]) - static_cast<std::uint8_t>(image_b[index + 1]));
            result.push_back(static_cast<std::uint8_t>(image_a[index + 2]) - static_cast<std::uint8_t>(image_b[index + 2]));
            result.push_back(0xff);

            index += 4;
        }

        writePNG(filename, result, width, height);
    }
}// namespace rosa