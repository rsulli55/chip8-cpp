
#include <SDL_events.h>
#include <glbinding/gl/enum.h>
#include <glbinding/gl/functions.h>
#include <glbinding/glbinding.h>
#include <glbinding/gl/gl.h>
#define IMGUI_IMPL_OPENGL_LOADER_GLBINDING3 1
#include <SDL.h>
#include <SDL_video.h>
#include "spdlog/spdlog.h"

#include "common.h"

using namespace gl;

void  dbg_callback(GLenum source,
                 GLenum type,
                 GLuint id,
                 GLenum severity,
                 GLsizei length,
                 const GLchar* message,
                 const void* userParam ) {
    spdlog::debug("GL CALLBACK: {}, severity = {:x}, message = {}",
            (type == GL_DEBUG_TYPE_ERROR ? "GL Error" : ""),
            severity, message);
}


int main() {
    spdlog::set_level(spdlog::level::debug);

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        spdlog::error("Could not initialize SDL\nError: %s\n", SDL_GetError());
        return 10;
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,
                        SDL_GL_CONTEXT_PROFILE_CORE);

    SDL_Window *window = SDL_CreateWindow(
        "OpenGL window", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 600,
        400, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
    SDL_GLContext gl_context = SDL_GL_CreateContext(window);
    SDL_GL_MakeCurrent(window, gl_context);
    SDL_GL_SetSwapInterval(1);

    glbinding::initialize([](const char *name) {
        return (glbinding::ProcAddress)SDL_GL_GetProcAddress(name);
    });

    glEnable(GLenum::GL_DEBUG_OUTPUT);
    glDebugMessageCallback(dbg_callback, 0);

    // make frame buffer
    GLuint fbo;
    glGenFramebuffers(1, &fbo);

    // make texture
    GLuint texture;
    glGenTextures(1, &texture);

    std::array<u8, 24> screen_data { 
        128u, 128u, 128u, 255u,  200u, 20u, 20u, 255u,  20u, 20u, 200u, 255u,
        20u, 20u, 20u, 255u,     20u, 200u, 20u, 255u,  200u, 200u, 200u, 255u
    };

    // configure texture
    glBindTexture(GLenum::GL_TEXTURE_2D, texture);
    glTexParameteri(GLenum::GL_TEXTURE_2D, GLenum::GL_TEXTURE_MIN_FILTER, GLenum::GL_NEAREST);
    glTexParameteri(GLenum::GL_TEXTURE_2D, GLenum::GL_TEXTURE_MAG_FILTER, GLenum::GL_NEAREST);
    glTexParameteri(GLenum::GL_TEXTURE_2D, GLenum::GL_TEXTURE_WRAP_S, GLenum::GL_CLAMP_TO_BORDER);
    glTexParameteri(GLenum::GL_TEXTURE_2D, GLenum::GL_TEXTURE_WRAP_T, GLenum::GL_CLAMP_TO_BORDER);
    // glGenerateMipmap(GLenum::GL_TEXTURE_2D);
    glTexStorage2D(GLenum::GL_TEXTURE_2D, 1, GLenum::GL_RGBA8, 3, 2);
    glTexSubImage2D(GLenum::GL_TEXTURE_2D, 0, 0, 0, 3, 2,
            GLenum::GL_BGRA, GLenum::GL_UNSIGNED_BYTE, screen_data.data());
    glBindTexture(GLenum::GL_TEXTURE_2D, 0);

    // configre framebuffer
    glBindFramebuffer(GLenum::GL_FRAMEBUFFER, fbo);
    glViewport(0, 0, 600, 400);
    glFramebufferTexture2D(GLenum::GL_FRAMEBUFFER, GLenum::GL_COLOR_ATTACHMENT0, GLenum::GL_TEXTURE_2D, texture, 0);
    GLenum status = glCheckFramebufferStatus(GLenum::GL_FRAMEBUFFER);
    if (status != GLenum::GL_FRAMEBUFFER_COMPLETE) 
        spdlog::debug("Status of fbo: {}", status);

    glBindFramebuffer(GLenum::GL_FRAMEBUFFER, 0);

    bool running = true;
    auto steps = std::array<int, 6> {0, 1, 2, 3, 4, 5};
    auto counter = 0;
    auto step = 0;
    float color = 0.07f;

    while (running) {
        counter++;
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                    running = false;
            }
        }

        if (counter >= 60) {
            for (int i = 0; i < 5; ++i) {
                screen_data[i] = screen_data[i+1];
            }
            screen_data[5] = screen_data[0];
            counter = 0;
        } 

        // bind texture
        glBindTexture(GLenum::GL_TEXTURE_2D, texture);

        // load up new framebuffer
        glBindFramebuffer(GLenum::GL_FRAMEBUFFER, fbo);
        // color background
        /* glClearColor(color, 0.13f, 0.17f, 1.0f); */
        /* // clean back buffer and assign color to it */
        /* glClear(GL_COLOR_BUFFER_BIT); */
        // blit framebuffer
        glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo);
        glReadBuffer(GLenum::GL_COLOR_ATTACHMENT0);
        status = glCheckFramebufferStatus(GLenum::GL_READ_FRAMEBUFFER);
        if (status != GLenum::GL_FRAMEBUFFER_COMPLETE) 
            spdlog::debug("Status of READ fbo: {}", status);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
        status = glCheckFramebufferStatus(GLenum::GL_DRAW_FRAMEBUFFER);
        if (status != GLenum::GL_FRAMEBUFFER_COMPLETE) 
            spdlog::debug("Status of DRAW framebuffer: {}", status);
        /* glDrawBuffer(GLenum::GL_BACK); */
        glBlitFramebuffer(0, 0, 600, 400, 0, 0, 600, 400,  
                GL_COLOR_BUFFER_BIT,  GLenum::GL_NEAREST);

        glBindFramebuffer(GLenum::GL_FRAMEBUFFER, 0);
        // swap buffers
        SDL_GL_SwapWindow(window);
    }

    // free objects
    glDeleteFramebuffers(1, &fbo);
    glDeleteTextures(1, &texture);

    SDL_GL_DeleteContext(gl_context);
    SDL_DestroyWindow(window);
    SDL_Quit();



}






