
#include "imgui.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_sdl.h"

#include "window.h"

#define IMGUI_IMPL_OPENGL_LOADER_GLBINDING3 1

Window::Window(u8 screen_scale) :
    screen_scale_{screen_scale},
    window_width_{Chip8::SCREEN_WIDTH * screen_scale},
    window_height_{Chip8::SCREEN_HEIGHT * screen_scale} {
        u32 ec = init_video();
        if (ec!= 0) {
            spdlog::error("initializing video failed, exiting.");
            std::terminate();
        }
    }

Window::~Window() {
    spdlog::debug("Calling ImGui_ImplOpenGL3_Shutdown");
    ImGui_ImplOpenGL3_Shutdown();
    spdlog::debug("Calling ImGui_ImplSDL2_Shutdown");
    ImGui_ImplSDL2_Shutdown();
    spdlog::debug("Calling ImGui::DestroyContext");
    ImGui::DestroyContext();

    spdlog::debug("Deleting GL Context");
    SDL_GL_DeleteContext(gl_context_);
    spdlog::debug("Deleting SDL Window");
    SDL_DestroyWindow(window_);
    spdlog::debug("Calling SDL Quit");
    SDL_Quit();
}

void Window::resize() {
    int width = 0;
    int height = 0;
    SDL_GL_GetDrawableSize(window_, &width, &height);
    glViewport(0, 0, width, height);
}


u32 Window::init_video() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        spdlog::error("Could not initialize SDL\nError: %s\n", SDL_GetError());
        return 10;
    }

    // configure OpenGL settings
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,
                        SDL_GL_CONTEXT_PROFILE_CORE);

    // create window and GL context
    SDL_WindowFlags window_flags = static_cast<SDL_WindowFlags>(SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
    window_ = SDL_CreateWindow(WINDOW_NAME, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
            window_width_, window_height_, window_flags);
    if (!window_) {
        spdlog::error("Window could not be created!\nError: %s\n",
                      SDL_GetError());
        return 20;
    }
    gl_context_ = SDL_GL_CreateContext(window_);
    SDL_GL_MakeCurrent(window_, gl_context_);
    SDL_GL_SetSwapInterval(1); // Enable vsync

    glbinding::initialize([](const char* name) { 
            return (glbinding::ProcAddress)SDL_GL_GetProcAddress(name); 
            });

    // enable debug output
    glEnable(GLenum::GL_DEBUG_OUTPUT);
    glDebugMessageCallback(gl_dbg_callback, 0);

    // set up transparency
    glEnable(GLenum::GL_BLEND);
    glBlendFunc(GLenum::GL_SRC_ALPHA, GLenum::GL_ONE_MINUS_SRC_ALPHA);


    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    // Setup Platform/Renderer backends
    ImGui_ImplSDL2_InitForOpenGL(window_, gl_context_);
    ImGui_ImplOpenGL3_Init(glsl_version);

    return 0;
}




