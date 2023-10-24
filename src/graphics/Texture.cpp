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
 *  You should have received a copy of the GNU General Public License along with bbai. If not,
 *  see <https://www.gnu.org/licenses/>.
 */

#include <graphics/Texture.hpp>
#include <graphics/gl.h>
#include <GLFW/glfw3.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <physfs.h>

#include <spdlog/spdlog.h>

namespace rosa {
    auto Texture::loadFromPhysFS(const std::string& name) -> void {

        if (PHYSFS_exists(name.c_str()) == 0) {
            spdlog::error("Couldn't load asset: {}", name);
            return;
        }

        PHYSFS_file* myfile = PHYSFS_openRead(name.c_str());
        //char *buffer = new char[PHYSFS_fileLength(myfile)];
        //int length_read = PHYSFS_readBytes (myfile, buffer, PHYSFS_fileLength(myfile));
        //assert(length_read == PHYSFS_fileLength(myfile));

        // Check for DDS header
        char filecode[4];
        PHYSFS_readBytes(myfile, filecode, 4);
        if (strncmp(filecode, "DDS ", 4) != 0) {
            spdlog::error("DDS file didn't contain a DDS header: {}", name);
            return;
        }

        // get the surface desc
        unsigned char header[124];
        PHYSFS_readBytes(myfile, header, 124);

        // Decode header
        unsigned int height      = *(unsigned int*)&(header[8 ]);
        unsigned int width         = *(unsigned int*)&(header[12]);
        unsigned int linearSize     = *(unsigned int*)&(header[16]);
        unsigned int mipMapCount = *(unsigned int*)&(header[24]);
        unsigned int fourCC      = *(unsigned int*)&(header[80]);
        m_size = glm::vec2(width, height);

        unsigned char * buffer;
        unsigned int bufsize;

        /* how big is it going to be including all mipmaps? */
        bufsize = mipMapCount > 1 ? linearSize * 2 : linearSize;

        // Read the remainder
        buffer = (unsigned char*)malloc(bufsize * sizeof(unsigned char));
        PHYSFS_readBytes(myfile, buffer, bufsize);

        // Close up physfs stream
        PHYSFS_close(myfile);

        // Process DDS
        unsigned int components  = (fourCC == FOURCC_DXT1) ? 3 : 4;
        unsigned int format;
        switch(fourCC)
        {
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
            free(buffer);
            return;
        }

        // Create one OpenGL texture
        glGenTextures(1, &m_texture_id);

        // "Bind" the newly created texture : all future texture functions will modify this texture
        glBindTexture(GL_TEXTURE_2D, m_texture_id);

        unsigned int blockSize = (format == GL_COMPRESSED_RGBA_S3TC_DXT1_EXT) ? 8 : 16;
        unsigned int offset = 0;

        /* load the mipmaps */
        for (unsigned int level = 0; level < mipMapCount && (width || height); ++level)
        {
            unsigned int size = ((width+3)/4)*((height+3)/4)*blockSize;
            glCompressedTexImage2D(GL_TEXTURE_2D, level, format, width, height, 
                0, size, buffer + offset);

            offset += size;
            width  /= 2;
            height /= 2;
        }

        free(buffer); 
    }

} // namespace rosa