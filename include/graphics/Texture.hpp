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

/*! \file */

#pragma once

#include <core/Resource.hpp>
#include <core/Uuid.hpp>
#include <glm/glm.hpp>
#include <graphics/gl.hpp>
#include <string>

namespace rosa {

    /**
     * \brief All available texture filter modes for OpenGL
     */
    enum TextureFilterMode {
        Nearest              = GL_NEAREST,
        Linear               = GL_LINEAR,
        NearestMipMapNearest = GL_NEAREST_MIPMAP_NEAREST,
        NearestMipMapLinear  = GL_NEAREST_MIPMAP_LINEAR,
        LinearMipMapNearest  = GL_LINEAR_MIPMAP_NEAREST,
        LinearMipMapLinear   = GL_LINEAR_MIPMAP_LINEAR
    };

    /**
     * \brief FilterParams comprise of a TextureFilterMode for both magnify and minify
     */
    struct TextureFilterParams {
        TextureFilterMode minify{TextureFilterMode::NearestMipMapLinear};
        TextureFilterMode magnify{TextureFilterMode::Linear};
    };

    /**
     * \brief The DDS texture is malformed
     *
     * Throw when failing to load a DDS texture
     */
    class MalformedDDSException : public Exception {
    public:
        explicit MalformedDDSException(const std::string& msg)
            : Exception(msg) {}
    };

    /**
     * \brief Manages a DDS texture object
     *
     * When constructed a DDS texture is read from disk, decoded and stored in memory.
     * It is then uploaded to the GPU, including any mipmaps present in the texture.
     *
     * It is possible to load a texture with specific TextureFilterParams but it will
     * default to NearestMipMapLinear & Linear
     */
    class Texture : public Resource {
    public:
        /**
         * \brief Construct a texture object
         * \param name Filename relative to it's asset pack
         * \param uuid Uuid to associate with the texture
         * \param pack Mountpoint of the asset pack
         * \param filter_params Filtering parameters
         */
        Texture(const std::string& name, const Uuid& uuid, const std::string& pack, TextureFilterParams filter_params = {});

        /**
         * \brief Get the OpenGL Id of the texture on the GPU
         */
        auto getOpenGlId() const -> unsigned int {
            return m_texture_id;
        }

        auto getSize() const -> glm::vec2 {
            return m_size;
        }

    private:
        unsigned int        m_texture_id{0};
        glm::vec2           m_size{0.F, 0.F};
        TextureFilterParams m_filter_params;
    };

}// namespace rosa