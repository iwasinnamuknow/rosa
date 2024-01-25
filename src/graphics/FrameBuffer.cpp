///////////////////////////////////////////////////////////////////////////////
// FrameBuffer.cpp
// ===============
// class for OpenGL Frame Buffer Object (FBO)
// It contains a 32bit color buffer and a depth buffer as GL_DEPTH_COMPONENT24
// Call init() to create/resize a FBO with given width and height params.
// It supports MSAA (Multi Sample Anti Aliasing) FBO. If msaa=0, it creates a
// single-sampled FBO. If msaa > 0 (even number), it creates a multi-sampled
// FBO.
//
// NOTE: This class does not use GL_ARB_texture_multisample extension yet. Call
//       update() function explicitly to blit color/depth buffers from
//       multi-sample FBO to single-sample FBO if you want to get single-sample
//       color and depth data from MSAA FBO.
//
//  AUTHOR: Song Ho Ahn (song.ahn@gmail.com)
// CREATED: 2015-03-05
// UPDATED: 2022-05-21
///////////////////////////////////////////////////////////////////////////////

#include <sstream>
#include <graphics/FrameBuffer.hpp>
#include <cstdint>

namespace {
    constexpr unsigned int GL_LUMINANCE = 0x1909;
    constexpr unsigned int GL_LUMINANCE_ALPHA = 0x190A;
    //constexpr unsigned int GL_R3_G3_B2            = 0x2A10;
    constexpr unsigned int GL_ALPHA4 = 0x803B;
    constexpr unsigned int GL_ALPHA8 = 0x803C;
    constexpr unsigned int GL_ALPHA12 = 0x803D;
    constexpr unsigned int GL_ALPHA16 = 0x803E;
    constexpr unsigned int GL_LUMINANCE4 = 0x803F;
    constexpr unsigned int GL_LUMINANCE8 = 0x8040;
    constexpr unsigned int GL_LUMINANCE12 = 0x8041;
    constexpr unsigned int GL_LUMINANCE16 = 0x8042;
    constexpr unsigned int GL_LUMINANCE4_ALPHA4 = 0x8043;
    constexpr unsigned int GL_LUMINANCE6_ALPHA2 = 0x8044;
    constexpr unsigned int GL_LUMINANCE8_ALPHA8 = 0x8045;
    constexpr unsigned int GL_LUMINANCE12_ALPHA4 = 0x8046;
    constexpr unsigned int GL_LUMINANCE12_ALPHA12 = 0x8047;
    constexpr unsigned int GL_LUMINANCE16_ALPHA16 = 0x8048;
    constexpr unsigned int GL_INTENSITY = 0x8049;
    constexpr unsigned int GL_INTENSITY4 = 0x804A;
    constexpr unsigned int GL_INTENSITY8 = 0x804B;
    constexpr unsigned int GL_INTENSITY12 = 0x804C;
    constexpr unsigned int GL_INTENSITY16 = 0x804D;
} // namespace


namespace rosa {

    FrameBuffer::~FrameBuffer() {
        delete_buffers();
    }

    auto FrameBuffer::init(int width, int height, int msaa) -> bool {
        // check w/h
        if (width <= 0 || height <= 0) {
            m_error_message = "The buffer size is not positive.";
            return true;
        }

        // reset error message
        m_error_message = "no error";

        m_width = width;
        m_height = height;
        m_msaa = msaa;

        // reset buffers
        delete_buffers();

        // create arrays
        m_color_buffer.resize(static_cast<std::uint64_t>(width) * static_cast<std::uint64_t>(height) * 4);    // 32 bits per pixel
        m_depth_buffer.resize(static_cast<std::uint64_t>(width) * static_cast<std::uint64_t>(height));        // 32 bits per pixel

        // create single-sample FBO
        glGenFramebuffers(1, &m_fbo_id);
        glBindFramebuffer(GL_FRAMEBUFFER, m_fbo_id);

        // create a texture object to store colour info, and attach it to fbo
        glGenTextures(1, &m_texture_id);
        glBindTexture(GL_TEXTURE_2D, m_texture_id);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        //glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE); // automatic mipmap generation included in OpenGL v1.4
        glGenerateMipmap(m_texture_id);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_texture_id, 0);

        // create a renderbuffer object to store depth info, attach it to fbo
        glGenRenderbuffers(1, &m_rbo_id);
        glBindRenderbuffer(GL_RENDERBUFFER, m_rbo_id);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_rbo_id);

        // check FBO completeness
        bool status = check_frame_buffer_status();

        // create multi-sample fbo
        if (msaa > 0) {
            glGenFramebuffers(1, &m_fbo_msaa_id);
            glBindFramebuffer(GL_FRAMEBUFFER, m_fbo_msaa_id);

            // create a render buffer object to store colour info
            glGenRenderbuffers(1, &m_rbo_msaa_color_id);
            glBindRenderbuffer(GL_RENDERBUFFER, m_rbo_msaa_color_id);
            glRenderbufferStorageMultisample(GL_RENDERBUFFER, msaa, GL_RGBA8, width, height);

            // attach a renderbuffer to FBO color attachment point
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, m_rbo_msaa_color_id);

            // create a renderbuffer object to store depth info
            glGenRenderbuffers(1, &m_rbo_msaa_depth_id);
            glBindRenderbuffer(GL_RENDERBUFFER, m_rbo_msaa_depth_id);
            glRenderbufferStorageMultisample(GL_RENDERBUFFER, msaa, GL_DEPTH_COMPONENT, width, height);

            // attach a renderbuffer to FBO depth attachment point
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_rbo_msaa_depth_id);

            // check FBO completeness again
            status = check_frame_buffer_status();
        }

        // unbind
        glBindRenderbuffer(GL_RENDERBUFFER, 0);
        glBindTexture(GL_TEXTURE_2D, 0);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        return status;
    }

    auto FrameBuffer::delete_buffers() -> void {
        if (m_rbo_msaa_color_id > 0) {
            glDeleteRenderbuffers(1, &m_rbo_msaa_color_id);
            m_rbo_msaa_color_id = 0;
        }
        if (m_rbo_msaa_depth_id > 0) {
            glDeleteRenderbuffers(1, &m_rbo_msaa_depth_id);
            m_rbo_msaa_depth_id = 0;
        }
        if (m_fbo_msaa_id > 0) {
            glDeleteFramebuffers(1, &m_fbo_msaa_id);
            m_fbo_msaa_id = 0;
        }
        if (m_texture_id > 0) {
            glDeleteTextures(1, &m_texture_id);
            m_texture_id = 0;
        }
        if (m_rbo_id > 0) {
            glDeleteRenderbuffers(1, &m_rbo_id);
            m_rbo_id = 0;
        }
        if (m_fbo_id > 0) {
            glDeleteFramebuffers(1, &m_fbo_id);
            m_fbo_id = 0;
        }

        m_color_buffer.clear();
        m_depth_buffer.clear();
    }

    auto FrameBuffer::getId() const -> GLuint {
        if (m_msaa == 0) {
            return m_fbo_id;
        }

        return m_fbo_msaa_id;
    }

    auto FrameBuffer::bind() -> void {
        if (m_msaa == 0) {
            glBindFramebuffer(GL_FRAMEBUFFER, m_fbo_id);
        } else {
            glBindFramebuffer(GL_FRAMEBUFFER, m_fbo_msaa_id);
        }
    }

    auto FrameBuffer::update() -> void {
        if (m_msaa > 0) {
            // blit color buffer
            glBindFramebuffer(GL_READ_FRAMEBUFFER, m_fbo_msaa_id);
            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_fbo_id);
            glBlitFramebuffer(0, 0, m_width, m_height,
                              0, 0, m_width, m_height,
                              GL_COLOR_BUFFER_BIT,
                              GL_LINEAR);

            //NOTE: blit separately depth buffer because different scale filter
            //NOTE: scale filter for depth buffer must be GL_NEAREST, otherwise, invalid op
            glBlitFramebuffer(0, 0, m_width, m_height,
                              0, 0, m_width, m_height,
                              GL_DEPTH_BUFFER_BIT,
                              GL_NEAREST);
        }

        // also, generate mipmaps for color buffer (texture)
        glBindTexture(GL_TEXTURE_2D, m_texture_id);
        glGenerateMipmap(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    auto FrameBuffer::unbind() -> void {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    auto FrameBuffer::blitColorTo(GLuint fbo_id, int start_x, int start_y, int width, int height) -> void {
        // if width/height not specified, use src dimension
        if (width == 0) {
            width = m_width;
        }

        if (height == 0) {
            height = m_height;
        }

        GLuint srcId = (m_msaa == 0) ? m_fbo_id : m_fbo_msaa_id;
        glBindFramebuffer(GL_READ_FRAMEBUFFER, srcId);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo_id);
        glBlitFramebuffer(0, 0, m_width, m_height,  // src rect
                          start_x, start_y, width, height,              // dst rect
                          GL_COLOR_BUFFER_BIT,              // buffer mask
                          GL_LINEAR);                       // scale filter
    }

    auto FrameBuffer::blitDepthTo(GLuint fbo_id, int start_x, int start_y, int width, int height) -> void {
        // if width/height not specified, use src dimension
        if (width == 0) {
            width = m_width;
        }

        if (height == 0) {
            height = m_height;
        }

        // NOTE: scale filter for depth buffer must be GL_NEAREST, otherwise, invalid op
        GLuint srcId = (m_msaa == 0) ? m_fbo_id : m_fbo_msaa_id;
        glBindFramebuffer(GL_READ_FRAMEBUFFER, srcId);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo_id);
        glBlitFramebuffer(0, 0, m_width, m_height,  // src rect
                          start_x, start_y, width, height,              // dst rect
                          GL_DEPTH_BUFFER_BIT,              // buffer mask
                          GL_NEAREST);                      // scale filter
    }

    auto FrameBuffer::copyColorBuffer() -> void {
        if (m_msaa > 0) {
            blitColorTo(m_fbo_id); // copy multi-sample to single-sample first
        }
        // store pixel data to internal array
        glBindFramebuffer(GL_READ_FRAMEBUFFER, m_fbo_id);
        glReadPixels(0, 0, m_width, m_height, GL_RGBA, GL_UNSIGNED_BYTE, m_color_buffer.data());

        // Flip the image data vertically
        for (int y = 0; y < m_height / 2; ++y) {
            int topOffset    = y * m_width * 4;
            int bottomOffset = (m_height - y - 1) * m_width * 4;

            for (int x = 0; x < m_width * 4; ++x) {
                std::swap(m_color_buffer[topOffset + x], m_color_buffer[bottomOffset + x]);
            }
        }
    }

    auto FrameBuffer::copyDepthBuffer() -> void {
        if (m_msaa > 0) {
            blitDepthTo(m_fbo_id);  // copy multi-sample to single-sample first
        }
        glBindFramebuffer(GL_READ_FRAMEBUFFER, m_fbo_id);
        glReadPixels(0, 0, m_width, m_height, GL_DEPTH_COMPONENT, GL_FLOAT, m_depth_buffer.data());
    }

    auto FrameBuffer::check_frame_buffer_status() -> bool {
        // check FBO status
        GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
        switch (status) {
            case GL_FRAMEBUFFER_COMPLETE:
                m_error_message = "no error";
                return true;

            case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
                m_error_message = "[ERROR] Framebuffer incomplete: Attachment is NOT complete.";
                return false;

            case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
                m_error_message = "[ERROR] Framebuffer incomplete: No image is attached to FBO.";
                return false;
            case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
                m_error_message = "[ERROR] Framebuffer incomplete: Draw buffer.";
                return false;

            case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
                m_error_message = "[ERROR] Framebuffer incomplete: Read buffer.";
                return false;

            case GL_FRAMEBUFFER_UNSUPPORTED:
                m_error_message = "[ERROR] Framebuffer incomplete: Unsupported by FBO implementation.";
                return false;

            default:
                m_error_message = "[ERROR] Framebuffer incomplete: Unknown error.";
                return false;
        }
    }

    auto FrameBuffer::getStatus() const -> std::string {
        if (m_msaa == 0) {
            glBindFramebuffer(GL_FRAMEBUFFER, m_fbo_id);
        } else {
            glBindFramebuffer(GL_FRAMEBUFFER, m_fbo_msaa_id);
        }

        std::stringstream ss;

        ss << "\n===== FBO STATUS =====\n";

        // print max # of colorbuffers supported by FBO
        int colorBufferCount = 0;
        glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, &colorBufferCount);
        ss << "Max Number of Color Buffer Attachment Points: " << colorBufferCount << "\n";

        // get max # of multi samples
        int multiSampleCount = 0;
        glGetIntegerv(GL_MAX_SAMPLES, &multiSampleCount);
        ss << "Max Number of Samples for MSAA: " << multiSampleCount << "\n";

        int objectType = 0;
        int objectId = 0;

        // print info of the colorbuffer attachable image
        for (int i = 0; i < colorBufferCount; ++i) {
            glGetFramebufferAttachmentParameteriv(GL_FRAMEBUFFER,
                                                  GL_COLOR_ATTACHMENT0 + static_cast<unsigned int>(i),
                                                  GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE,
                                                  &objectType);
            if (objectType != GL_NONE) {
                glGetFramebufferAttachmentParameteriv(GL_FRAMEBUFFER,
                                                      GL_COLOR_ATTACHMENT0 + static_cast<unsigned int>(i),
                                                      GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME,
                                                      &objectId);

                std::string formatName;

                ss << "Color Attachment " << i << ": ";
                if (objectType == GL_TEXTURE) {
                    ss << "GL_TEXTURE, " << FrameBuffer::get_texture_parameters(static_cast<GLuint>(objectId)) << "\n";
                } else if (objectType == GL_RENDERBUFFER) {
                    ss << "GL_RENDERBUFFER, " << FrameBuffer::get_renderbuffer_parameters(static_cast<GLuint>(objectId)) << "\n";
                }
            }
        }

        // print info of the depthbuffer attachable image
        glGetFramebufferAttachmentParameteriv(GL_FRAMEBUFFER,
                                              GL_DEPTH_ATTACHMENT,
                                              GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE,
                                              &objectType);
        if (objectType != GL_NONE) {
            glGetFramebufferAttachmentParameteriv(GL_FRAMEBUFFER,
                                                  GL_DEPTH_ATTACHMENT,
                                                  GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME,
                                                  &objectId);

            ss << "Depth Attachment: ";
            switch (objectType) {
                case GL_TEXTURE:
                    ss << "GL_TEXTURE, " << FrameBuffer::get_texture_parameters(static_cast<GLuint>(objectId)) << "\n";
                    break;
                case GL_RENDERBUFFER:
                    ss << "GL_RENDERBUFFER, " << FrameBuffer::get_renderbuffer_parameters(static_cast<GLuint>(objectId)) << "\n";
                    break;
            }
        }

        // print info of the stencilbuffer attachable image
        glGetFramebufferAttachmentParameteriv(GL_FRAMEBUFFER,
                                              GL_STENCIL_ATTACHMENT,
                                              GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE,
                                              &objectType);
        if (objectType != GL_NONE) {
            glGetFramebufferAttachmentParameteriv(GL_FRAMEBUFFER,
                                                  GL_STENCIL_ATTACHMENT,
                                                  GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME,
                                                  &objectId);

            ss << "Stencil Attachment: ";
            switch (objectType) {
                case GL_TEXTURE:
                    ss << "GL_TEXTURE, " << FrameBuffer::get_texture_parameters(static_cast<GLuint>(objectId)) << "\n";
                    break;
                case GL_RENDERBUFFER:
                    ss << "GL_RENDERBUFFER, " << FrameBuffer::get_renderbuffer_parameters(static_cast<GLuint>(objectId)) << "\n";
                    break;
            }
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        return ss.str();
    }

    auto FrameBuffer::get_texture_parameters(GLuint id) -> std::string {
        if (glIsTexture(id) == GL_FALSE) {
            return "Not texture object";
        }

        GLint width = 0;
        GLint height = 0;
        GLint format = 0;
        std::string formatName;
        glBindTexture(GL_TEXTURE_2D, id);
        glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &width);            // get texture width
        glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &height);          // get texture height
        glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_INTERNAL_FORMAT, &format); // get texture internal format
        glBindTexture(GL_TEXTURE_2D, 0);

        formatName = FrameBuffer::convert_internal_format_to_string(static_cast<GLuint>(format));

        std::stringstream ss;
        ss << width << "x" << height << ", " << formatName;
        return ss.str();
    }

    auto FrameBuffer::get_renderbuffer_parameters(GLuint id) -> std::string {
        if (glIsRenderbuffer(id) == GL_FALSE) {
            return "Not Renderbuffer object";
        }

        GLint width = 0;
        GLint height = 0;
        GLint format = 0;
        GLint samples = 0;
        std::string formatName;
        glBindRenderbuffer(GL_RENDERBUFFER, id);
        glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_WIDTH, &width);    // get renderbuffer width
        glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_HEIGHT, &height);  // get renderbuffer height
        glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_INTERNAL_FORMAT, &format); // get renderbuffer internal format
        glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_SAMPLES, &samples);   // get multisample count
        glBindRenderbuffer(GL_RENDERBUFFER, 0);

        formatName = FrameBuffer::convert_internal_format_to_string(static_cast<GLuint>(format));

        std::stringstream ss;
        ss << width << "x" << height << ", " << formatName << ", MSAA(" << samples << ")";
        return ss.str();
    }

    auto FrameBuffer::convert_internal_format_to_string(GLenum format) -> std::string {
        std::string formatName;

        switch (format) {
            case GL_STENCIL_INDEX:      // 0x1901
                formatName = "GL_STENCIL_INDEX";
                break;
            case GL_DEPTH_COMPONENT:    // 0x1902
                formatName = "GL_DEPTH_COMPONENT";
                break;
            case GL_ALPHA:              // 0x1906
                formatName = "GL_ALPHA";
                break;
            case GL_RGB:                // 0x1907
                formatName = "GL_RGB";
                break;
            case GL_RGBA:               // 0x1908
                formatName = "GL_RGBA";
                break;
            case GL_LUMINANCE:          // 0x1909
                formatName = "GL_LUMINANCE";
                break;
            case GL_LUMINANCE_ALPHA:    // 0x190A
                formatName = "GL_LUMINANCE_ALPHA";
                break;
            case GL_R3_G3_B2:           // 0x2A10
                formatName = "GL_R3_G3_B2";
                break;
            case GL_ALPHA4:             // 0x803B
                formatName = "GL_ALPHA4";
                break;
            case GL_ALPHA8:             // 0x803C
                formatName = "GL_ALPHA8";
                break;
            case GL_ALPHA12:            // 0x803D
                formatName = "GL_ALPHA12";
                break;
            case GL_ALPHA16:            // 0x803E
                formatName = "GL_ALPHA16";
                break;
            case GL_LUMINANCE4:         // 0x803F
                formatName = "GL_LUMINANCE4";
                break;
            case GL_LUMINANCE8:         // 0x8040
                formatName = "GL_LUMINANCE8";
                break;
            case GL_LUMINANCE12:        // 0x8041
                formatName = "GL_LUMINANCE12";
                break;
            case GL_LUMINANCE16:        // 0x8042
                formatName = "GL_LUMINANCE16";
                break;
            case GL_LUMINANCE4_ALPHA4:  // 0x8043
                formatName = "GL_LUMINANCE4_ALPHA4";
                break;
            case GL_LUMINANCE6_ALPHA2:  // 0x8044
                formatName = "GL_LUMINANCE6_ALPHA2";
                break;
            case GL_LUMINANCE8_ALPHA8:  // 0x8045
                formatName = "GL_LUMINANCE8_ALPHA8";
                break;
            case GL_LUMINANCE12_ALPHA4: // 0x8046
                formatName = "GL_LUMINANCE12_ALPHA4";
                break;
            case GL_LUMINANCE12_ALPHA12:// 0x8047
                formatName = "GL_LUMINANCE12_ALPHA12";
                break;
            case GL_LUMINANCE16_ALPHA16:// 0x8048
                formatName = "GL_LUMINANCE16_ALPHA16";
                break;
            case GL_INTENSITY:          // 0x8049
                formatName = "GL_INTENSITY";
                break;
            case GL_INTENSITY4:         // 0x804A
                formatName = "GL_INTENSITY4";
                break;
            case GL_INTENSITY8:         // 0x804B
                formatName = "GL_INTENSITY8";
                break;
            case GL_INTENSITY12:        // 0x804C
                formatName = "GL_INTENSITY12";
                break;
            case GL_INTENSITY16:        // 0x804D
                formatName = "GL_INTENSITY16";
                break;
            case GL_RGB4:               // 0x804F
                formatName = "GL_RGB4";
                break;
            case GL_RGB5:               // 0x8050
                formatName = "GL_RGB5";
                break;
            case GL_RGB8:               // 0x8051
                formatName = "GL_RGB8";
                break;
            case GL_RGB10:              // 0x8052
                formatName = "GL_RGB10";
                break;
            case GL_RGB12:              // 0x8053
                formatName = "GL_RGB12";
                break;
            case GL_RGB16:              // 0x8054
                formatName = "GL_RGB16";
                break;
            case GL_RGBA2:              // 0x8055
                formatName = "GL_RGBA2";
                break;
            case GL_RGBA4:              // 0x8056
                formatName = "GL_RGBA4";
                break;
            case GL_RGB5_A1:            // 0x8057
                formatName = "GL_RGB5_A1";
                break;
            case GL_RGBA8:              // 0x8058
                formatName = "GL_RGBA8";
                break;
            case GL_RGB10_A2:           // 0x8059
                formatName = "GL_RGB10_A2";
                break;
            case GL_RGBA12:             // 0x805A
                formatName = "GL_RGBA12";
                break;
            case GL_RGBA16:             // 0x805B
                formatName = "GL_RGBA16";
                break;
            case GL_DEPTH_COMPONENT16:  // 0x81A5
                formatName = "GL_DEPTH_COMPONENT16";
                break;
            case GL_DEPTH_COMPONENT24:  // 0x81A6
                formatName = "GL_DEPTH_COMPONENT24";
                break;
            case GL_DEPTH_COMPONENT32:  // 0x81A7
                formatName = "GL_DEPTH_COMPONENT32";
                break;
            case GL_DEPTH_STENCIL:      // 0x84F9
                formatName = "GL_DEPTH_STENCIL";
                break;
            case GL_RGBA32F:            // 0x8814
                formatName = "GL_RGBA32F";
                break;
            case GL_RGB32F:             // 0x8815
                formatName = "GL_RGB32F";
                break;
            case GL_RGBA16F:            // 0x881A
                formatName = "GL_RGBA16F";
                break;
            case GL_RGB16F:             // 0x881B
                formatName = "GL_RGB16F";
                break;
            case GL_DEPTH24_STENCIL8:   // 0x88F0
                formatName = "GL_DEPTH24_STENCIL8";
                break;
            default:
                std::stringstream ss;
                ss << "Unknown Format(0x" << std::hex << format << ")" << std::ends;
                formatName = ss.str();
        }

        return formatName;
    }


} // namespace rosa