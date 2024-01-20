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

#include <core/Resource.hpp>
#include <core/Uuid.hpp>
#include <glm/glm.hpp>
#include <graphics/gl.hpp>
#include <physfs.h>
#include <string>

#define FOURCC_DXT1 0x31545844// Equivalent to "DXT1" in ASCII
#define FOURCC_DXT3 0x33545844// Equivalent to "DXT3" in ASCII
#define FOURCC_DXT5 0x35545844// Equivalent to "DXT5" in ASCII

#ifndef GL_EXT_texture_compression_s3tc
#define GL_EXT_texture_compression_s3tc 1
#define GL_COMPRESSED_RGB_S3TC_DXT1_EXT 0x83F0
#define GL_COMPRESSED_RGBA_S3TC_DXT1_EXT 0x83F1
#define GL_COMPRESSED_RGBA_S3TC_DXT3_EXT 0x83F2
#define GL_COMPRESSED_RGBA_S3TC_DXT5_EXT 0x83F3
#endif

namespace rosa {

    enum TextureFilterMode {
        Nearest              = GL_NEAREST,
        Linear               = GL_LINEAR,
        NearestMipMapNearest = GL_NEAREST_MIPMAP_NEAREST,
        NearestMipMapLinear  = GL_NEAREST_MIPMAP_LINEAR,
        LinearMipMapNearest  = GL_LINEAR_MIPMAP_NEAREST,
        LinearMipMapLinear   = GL_LINEAR_MIPMAP_LINEAR
    };

    struct TextureFilterParams {
        TextureFilterMode minify{TextureFilterMode::NearestMipMapLinear};
        TextureFilterMode magnify{TextureFilterMode::Linear};
    };

    class MalformedDDSException : public Exception {
    public:
        explicit MalformedDDSException(const std::string& msg)
            : Exception(msg) {}
    };

    class Texture : public Resource {

    public:
        Texture(std::string name, Uuid uuid, std::string pack,
                TextureFilterParams filter_params = {})
            : rosa::Resource(std::move(name), uuid, std::move(pack)), m_filter_params(filter_params) {}

        auto getOpenGlId() const -> unsigned int {
            return m_texture_id;
        }

        auto getSize() const -> const glm::vec2 {
            return m_size;
        }

        auto loadFromPhysFS() -> void override;

    private:
        unsigned int        m_texture_id{0};
        glm::vec2           m_size{0.F, 0.F};
        TextureFilterParams m_filter_params;
    };

}// namespace rosa