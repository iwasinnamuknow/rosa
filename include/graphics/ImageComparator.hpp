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

#include <vector>
#include <lodepng.h>
#include <core/Exception.hpp>

namespace rosa {

    class ImageComparator {
        
        public:
            static auto compareEqualityBasic(const std::vector<unsigned char>& lhs, const std::vector<unsigned char>& rhs) -> bool {
                
                if (lhs.size() != rhs.size()) {
                    return false;
                }

                auto size = lhs.size();

                for (std::size_t index = 0; index < size; index++) {
                    if (lhs.at(index) != rhs.at(index)) {
                        return false;
                    }
                }

                return true;
            }

            static auto writePNG(const std::string& filename, std::vector<unsigned char>& image, unsigned width, unsigned height) -> void {
                //Encode the image
                unsigned error = lodepng::encode(filename, image, width, height);

                //if there's an error, display it
                if(error) {
                    throw Exception(fmt::format("PNG encoder error in {}: {} ({})", filename, error, lodepng_error_text(error)));
                }
            }

            static std::vector<unsigned char> readPNG(const std::string& filename) {
                std::vector<unsigned char> image; //the raw pixels
                unsigned width, height;

                //decode
                unsigned error = lodepng::decode(image, width, height, filename);

                if(error) {
                    throw Exception(fmt::format("PNG decoder error in {}: {} ({})", filename, error, lodepng_error_text(error)));
                }

                //the pixels are now in the vector "image", 4 bytes per pixel, ordered RGBARGBA..., use it as texture, draw it, ...
                return image;
            }

    };

} // namespace rosa