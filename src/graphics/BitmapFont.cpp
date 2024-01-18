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

#include <graphics/BitmapFont.hpp>
#include <core/ResourceManager.hpp>
#include <graphics/gl.hpp>
#include <graphics/Quad.hpp>
#include <graphics/Colour.hpp>
#include <GLFW/glfw3.h>
#include <cassert>
#include <vector>
#include <physfs.h>

enum BlendFlags {
    BFG_RS_NONE = 0x0,
    BFG_RS_ALPHA = 0x1,
    BFG_RS_RGB = 0x2,
    BFG_RS_RGBA = 0x4
};

namespace rosa {

    auto BitmapFont::loadFromPhysFS() -> bool {

        const auto& name = getName();

        if (PHYSFS_exists(name.c_str()) == 0) {
            throw ResourceNotFoundException(fmt::format("Couldn't locate resource {}", name));
        }

        PHYSFS_file* myfile = PHYSFS_openRead(name.c_str());
        //char *buffer = new char[PHYSFS_fileLength(myfile)];
        //int length_read = PHYSFS_readBytes (myfile, buffer, PHYSFS_fileLength(myfile));
        //assert(length_read == PHYSFS_fileLength(myfile));

        auto length = PHYSFS_fileLength(myfile);
        std::vector<char> data;
        data.resize(length);
        int length_read = PHYSFS_readBytes(myfile, data.data(), length);
        assert(length_read == length);

        // Check magic bytes
        if((unsigned char)data[0]!=0xBF || (unsigned char)data[1]!=0xF2) {
            return false;
        }

        // Grab the rest of the header
        memcpy(&m_image_x,&data[2],sizeof(int));
        memcpy(&m_image_y,&data[6],sizeof(int));
        memcpy(&m_cell_x,&data[10],sizeof(int));
        memcpy(&m_cell_y,&data[14],sizeof(int));

        char bpp = data[18];
        m_base_char = data[19];

        // Check filesize
        auto expected_length = static_cast<std::int64_t>((bf_map_data_offset) + ((m_image_x * m_image_y) * (bpp / 8)));
        if(length != expected_length) {
            return false;
        }

        // Calculate font params
        m_row_pitch = static_cast<float>(m_image_x) / static_cast<float>(m_cell_x);
        m_col_factor = static_cast<float>(m_cell_x) / static_cast<float>(m_image_y);
        m_row_factor = static_cast<float>(m_cell_y) / static_cast<float>(m_image_y);
        m_y_offset = m_cell_y;

        // Determine blending options based on BPP
        switch(bpp)
        {
            case 8: // Greyscale
                m_render_style = BFG_RS_ALPHA;
                break;

            case 24: // RGB
                m_render_style = BFG_RS_RGB;
                break;

            case 32: // RGBA
                m_render_style = BFG_RS_RGBA;
                break;

            default: // Unsupported BPP
                return false;
                break;
        }

        std::vector<char> image_data;
        image_data.resize((m_image_x * m_image_y) * (bpp * 8));

        // Grab char widths
        memcpy(m_widths.data(), &data[bf_width_data_offset],256);

        // Grab image data
        memcpy(image_data.data(), &data[bf_map_data_offset] , (m_image_x * m_image_y) * (bpp / 8));

        // Create Texture
        glGenTextures(1,&m_texture_id);
        glBindTexture(GL_TEXTURE_2D,m_texture_id);
        // Fonts should be rendered at native resolution so no need for texture filtering
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
        // Stop chararcters from bleeding over edges
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);

        // Tex creation params are dependent on BPP
        switch(m_render_style)
        {
            case BFG_RS_ALPHA:
                glTexImage2D(GL_TEXTURE_2D,0,GL_RED,m_image_x,m_image_y,0,GL_RED,GL_UNSIGNED_BYTE,image_data.data());
                break;

            case BFG_RS_RGB:
                glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,m_image_x,m_image_y,0,GL_RGB,GL_UNSIGNED_BYTE,image_data.data());
                break;

            case BFG_RS_RGBA:
                glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,m_image_x,m_image_y,0,GL_RGBA,GL_UNSIGNED_BYTE,image_data.data());
                break;
        }

        return true;
    }

    auto BitmapFont::print(const std::string& text, int pos_x, int pos_y, Colour colour) -> std::vector<Quad> {
        setCursor(pos_x, pos_y);
        return generateCharQuads(text, colour);
    }

    auto BitmapFont::getWidth(const std::string &text) -> int {

        int size{0};

        for (const auto character : text) {
            size += m_widths.at(character);
        }

        return size;
    }


    // private helpers

    auto BitmapFont::setCursor(int x, int y) -> void {
        m_cursor_x = x;
        m_cursor_y = y;
    }

    auto BitmapFont::bind() -> void {
        glBindTexture(GL_TEXTURE_2D,m_texture_id);
    }

    auto BitmapFont::setBlending() -> void {
        switch(m_render_style)
        {
            case BFG_RS_ALPHA: // 8Bit
                glBlendFunc(GL_SRC_ALPHA,GL_SRC_ALPHA);
                glEnable(GL_BLEND);
                break;

            case BFG_RS_RGB:   // 24Bit
                glDisable(GL_BLEND);
                break;

            case BFG_RS_RGBA:  // 32Bit
                glBlendFunc(GL_ONE,GL_ONE_MINUS_SRC_ALPHA);
                glEnable(GL_BLEND);
                break;
        }
    }

    auto BitmapFont::select() -> void {
        bind();
        setBlending();
    }

    auto BitmapFont::reverseYAxis(bool state) -> void {
        if(state) {
            m_y_offset = -m_cell_y;
        } else {
            m_y_offset = m_cell_y;
        }

        m_invert_y_axis = state;
    }

    auto BitmapFont::generateCharQuads(const std::string& text, Colour colour) -> std::vector<Quad> {

        std::vector<Quad> quads{};

        for (const auto character : text) {

            int row = static_cast<int>(character - m_base_char) / m_row_pitch;
            int col = static_cast<int>(character - m_base_char) - row * m_row_pitch;

            Quad new_quad{};

            new_quad.size.x = static_cast<float>(m_cell_x);
            new_quad.size.y = static_cast<float>(m_cell_y);

            new_quad.pos.x = static_cast<float>(m_cursor_x);
            new_quad.pos.y = static_cast<float>(m_cursor_y);

            new_quad.colour = colour;

            new_quad.texture_id = m_texture_id;

            new_quad.texture_rect_size.x = static_cast<float>(m_cell_x) / static_cast<float>(m_image_x);
            new_quad.texture_rect_size.y = static_cast<float>(m_cell_y) / static_cast<float>(m_image_y);

            new_quad.texture_rect_pos.x = (static_cast<float>(col) * m_cell_x) / static_cast<float>(m_image_x);
            new_quad.texture_rect_pos.y = (static_cast<float>(row) * m_cell_y) / static_cast<float>(m_image_y);

            quads.push_back(new_quad);
            m_cursor_x += m_widths.at(character);
        }

        return quads;
    }

} // namespace rosa