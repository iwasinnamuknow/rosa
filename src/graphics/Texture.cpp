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

#include <GLFW/glfw3.h>
#include <graphics/Texture.hpp>
#include <graphics/gl.hpp>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <physfs.h>

#include <spdlog/spdlog.h>

namespace rosa {
    auto Texture::loadFromPhysFS() -> void {

        const auto& name = getName();

        if (PHYSFS_exists(name.c_str()) == 0) {
            throw ResourceNotFoundException(fmt::format("Couldn't locate resource {}", name));
        }

        PHYSFS_file* file = PHYSFS_openRead(name.c_str());

        auto length = PHYSFS_fileLength(file);
        if (length < 128) {
            PHYSFS_close(file);
            throw MalformedDDSException("Too small to be a DDS");
        }

        // Check for DDS header
        std::array<char, 4> file_code{};
        PHYSFS_readBytes(file, file_code.data(), 4);
        if (strncmp(file_code.data(), "DDS ", 4) != 0) {
            PHYSFS_close(file);
            throw MalformedDDSException("Missing DDS header");
        }

        // get the surface desc
        std::array<unsigned char, 124> header{};
        PHYSFS_readBytes(file, header.data(), 124);

        // Decode header
        unsigned int height{0};
        unsigned int width{0};
        unsigned int linear_size{0};
        unsigned int mip_map_count{0};
        unsigned int four_cc{0};

        // clang-format off
        std::memcpy(&height,        &header[8],  sizeof(unsigned int));
        std::memcpy(&width,         &header[12], sizeof(unsigned int));
        std::memcpy(&linear_size,   &header[16], sizeof(unsigned int));
        std::memcpy(&mip_map_count, &header[24], sizeof(unsigned int));
        std::memcpy(&four_cc,       &header[80], sizeof(unsigned int));
        // clang-format on

        m_size = glm::vec2(width, height);
        std::vector<unsigned char> buffer;

        /* how big is it going to be including all mipmaps? */
        unsigned int buf_size{mip_map_count > 1 ? linear_size * 2 : linear_size};

        // Read the remainder
        buffer.resize(buf_size);
        PHYSFS_readBytes(file, buffer.data(), buf_size);

        // Close up physfs stream
        PHYSFS_close(file);

        // Process DDS
        unsigned int format{0};
        switch (four_cc) {
            case FOURCC_DXT1:
                format = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
                break;
            case FOURCC_DXT3:
                format = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
                break;
            case FOURCC_DXT5:
                format = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
                break;
            default:
                throw MalformedDDSException("Could not determine DDS texture compression");
                break;
        }

        if (glfwGetCurrentContext() != nullptr) {
            // Create one OpenGL texture
            glGenTextures(1, &m_texture_id);

            // "Bind" the newly created texture : all future texture functions will modify this texture
            glBindTexture(GL_TEXTURE_2D, m_texture_id);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, m_filter_params.minify);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, m_filter_params.magnify);
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

            unsigned int block_size = (format == GL_COMPRESSED_RGBA_S3TC_DXT1_EXT) ? 8 : 16;
            unsigned int offset     = 0;

            /* load the mipmaps */
            for (unsigned int level = 0; level < mip_map_count && (width > 0 || height > 0); ++level) {
                auto size = ((width + 3) / 4) * ((height + 3) / 4) * block_size;
                glCompressedTexImage2D(
                        GL_TEXTURE_2D,
                        static_cast<GLint>(level),
                        format,
                        static_cast<GLsizei>(width),
                        static_cast<GLsizei>(height),
                        0, static_cast<GLsizei>(size),
                        buffer.data() + offset);

                offset += size;
                width /= 2;
                height /= 2;
            }
        }
    }

}// namespace rosa