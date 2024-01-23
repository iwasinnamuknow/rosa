
#pragma once

#include <graphics/gl.hpp>
#include <GLFW/glfw3.h>

#include <string>
#include <vector>
#include <span>

namespace rosa {

    /**
     * \brief FrameBuffer for OpenGL
     *
     * Originally written by Song Ho Ahn (song.ahn@gmail.com):
     *
     * It contains a 32bit color buffer and a depth buffer as GL_DEPTH_COMPONENT24
     * Call init() to create/resize a FBO with given width and height params.
     * It supports MSAA (Multi Sample Anti Aliasing) FBO. If msaa=0, it creates a
     * single-sampled FBO. If msaa > 0 (even number), it creates a multi-sampled
     * FBO.
     *
     * NOTE: This class does not use GL_ARB_texture_multisample extension yet. Call
     *  update() function explicitly to blit color/depth buffers from
     *  multi-sample FBO to single-sample FBO if you want to get single-sample
     *  color and depth data from MSAA FBO.
     */
    class FrameBuffer {
    public:

        FrameBuffer() = default;
        ~FrameBuffer();

        FrameBuffer(const FrameBuffer&) = delete;
        auto operator=(const FrameBuffer&) -> FrameBuffer& = delete;

        FrameBuffer(FrameBuffer&&) = delete;
        auto operator=(FrameBuffer&&) -> FrameBuffer& = delete;

        /**
         * \brief Initialise buffers
         * \param width window width
         * \param height window height
         * \param msaa number of multi-samples
         * \return true for successful initialisation, false otherwise
         */
        auto init(int width, int height, int msaa = 0) -> bool;

        /**
         * \brief Bind the FBO
         *
         * The FrameBuffer must be bound before being used
         */
        auto bind() -> void;

        /**
         * \brief Copy multi-sample to single-sample and generate mipmaps
         *
         * Explicitly blit color/depth buffer from multi-sample fbo to single-sample fbo.
         * This call is necessary to update the single-sample color/depth buffer and to
         * generate mipmaps explicitly.
         */
        auto update() -> void;

        /**
         * \brief Unbind the FBO
         *
         * Unbind when render operations are complete and the FBO has been copied
         */
        auto unbind() -> void;

        /**
         * \brief Blit colour of a region to a specific FBO
         * \param fbo_id destination FBO
         * \param start_x rect start x
         * \param start_y rect start y
         * \param width rect width
         * \param height rect height
         */
        auto blitColorTo(GLuint fbo_id, int start_x = 0, int start_y = 0, int width = 0, int height = 0) -> void;

        /**
         * \brief Blit depth of a region to a specific FBO
         * \param fbo_id destination FBO
         * \param start_x rect start x
         * \param start_y rect start y
         * \param width rect width
         * \param height rect height
         */
        auto blitDepthTo(GLuint fbo_id, int start_x = 0, int start_y = 0, int width = 0, int height = 0) -> void;

        /**
         * \brief Copy the colour buffer from GPU to CPU
         *
         * Read the pixels (32bits) from the color buffer and save data to the internal array.
         * If MSAA > 0, copy multi-sample colour buffer to single-sample buffer first
         */
        void copyColorBuffer();

        /**
         * \brief Copy the depth buffer from GPU to CPU
         *
         * Read the pixels from the depth buffer and save data to array.
         * If MSAA > 0, copy multi-sample depth buffer to single-sample buffer first
         */
        void copyDepthBuffer();

        /**
         * \brief Get the colour buffer
         * \return an array pointer of colour data
         */
        auto getColorBuffer() -> std::span<unsigned char> {
            return m_color_buffer;
        }

        /**
         * \brief Get the depth buffer
         * \return an array pointer of depth data
         */
        auto getDepthBuffer() -> std::span<float> {
            return m_depth_buffer;
        }

        /**
         * \brief Get the FBO ID
         * \return If MSAA is used, it will return the multi-sample FBO, otherwise it will return single-sample
         */
        auto getId() const -> GLuint;

        /**
         * \brief Get the ID of the single-sample colour buffer object
         * \return object ID
         */
        auto getColorId() const -> GLuint {
            return m_texture_id;
        }

        /**
         * \brief Get the ID of the single-sample depth RBO
         * \return object ID
         */
        auto getDepthId() const -> GLuint {
            return m_rbo_id;
        }

        /**
         * \brief Get the FrameBuffer width
         */
        auto getWidth() const -> int {
            return m_width;
        }

        /**
         * \brief Get the FrameBuffer height
         */
        auto getHeight() const -> int {
            return m_height;
        }

        /**
         * \brief Get the MSAA multiplier
         */
        auto getMsaa() const -> int {
            return m_msaa;
        }

        /**
         * \brief Check the status of the FBO
         * \return A string containing status information
         */
        auto getStatus() const -> std::string;

        /**
         * \brief Get error message(s)
         */
        auto getErrorMessage() const -> std::string {
            return m_error_message;
        }

    private:
        /**
         * \brief Clear buffers
         */
        auto delete_buffers() -> void;

        /**
         * \brief Check status of FBO setup
         * \return true for correct setup, false for incorrect
         */
        auto check_frame_buffer_status() -> bool;

        /**
         * \brief Get texture parameters as string using glGetTexLevelParameteriv()
         * \param id Texture object ID
         * \return A string containing texture parameters
         */
        static auto get_texture_parameters(GLuint id) -> std::string;

        /**
         * \brief Get renderbuffer parameters as string using glGetRenderbufferParameteriv
         * \param id RBO object ID
         * \return A string containing RBO parameters
         */
        static auto get_renderbuffer_parameters(GLuint id) -> std::string;

        /**
         * \brief Convert OpenGL internal format enum to string
         * \param format OpenGL format enum
         * \return A string representation of the OpenGL format
         */
        static auto convert_internal_format_to_string(GLenum format) -> std::string;

        int m_width{0};                              // buffer width
        int m_height{0};                             // buffer height
        int m_msaa{0};                               // # of multi samples; 0, 2, 4, 8,...
        std::vector<unsigned char> m_color_buffer{}; // color buffer (rgba)
        std::vector<float> m_depth_buffer{};         // depth buffer
        GLuint m_fbo_msaa_id{0};                     // primary id for multisample FBO
        GLuint m_rbo_msaa_color_id{0};               // id for multisample RBO (color buffer)
        GLuint m_rbo_msaa_depth_id{0};               // id for multisample RBO (depth buffer)
        GLuint m_fbo_id{0};                          // secondary id for frame buffer object
        GLuint m_texture_id{0};                      // id for texture object (color buffer)
        GLuint m_rbo_id{0};                          // id for render buffer object (depth buffer)
        std::string m_error_message{"no error"};  // error message, if any
    };

} // namespace rosa
