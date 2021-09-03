#include "emu.h"
#include "SDL.h"
#include "glbinding/glbinding.h"
#include "imgui.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_sdl.h"

u32 Emu::init_video() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        spdlog::error("Could not initialize SDL\nError: %s\n", SDL_GetError());
        return 10;
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,
                        SDL_GL_CONTEXT_PROFILE_CORE);

    SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
    SDL_Window* window = SDL_CreateWindow(WINDOW_NAME, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
            window_width_, window_height_, window_flags);

    if (!window_) {
        spdlog::error("Window could not be created!\nError: %s\n",
                      SDL_GetError());
        return 20;
    }

    SDL_GLContext gl_context = SDL_GL_CreateContext(window);
    SDL_GL_MakeCurrent(window, gl_context);
    SDL_GL_SetSwapInterval(1); // Enable vsync

    glbinding::initialize([](const char* name) { return (glbinding::ProcAddress)SDL_GL_GetProcAddress(name); });

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    // Setup Platform/Renderer backends
    ImGui_ImplSDL2_InitForOpenGL(window, gl_context);
    ImGui_ImplOpenGL3_Init(glsl_version);

    return 0;
}

Emu::Emu(u8 screen_scale, State state)
    : screen_scale_{screen_scale}, state_{state},
      window_width_{chip8_.SCREEN_WIDTH * screen_scale},
      window_height_{chip8_.SCREEN_HEIGHT * screen_scale} {
    u32 ec = init_video();
    // terminate if SDL does not load correctly
    if (ec != 0) {
        std::terminate();
    }
}

Emu::~Emu() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    SDL_GL_DeleteContext(gl_context_);
    SDL_DestroyWindow(window_);
    SDL_Quit();
}

void Emu::render() {
    const auto &screen = chip8_.screen();
    renderer_.render(screen);
/*     SDL_RenderClear(renderer_); */
/*     SDL_SetRenderDrawColor(renderer_, background_red, background_green, */
/*                            background_blue, 0); */
/*     SDL_RenderFillRect(renderer_, nullptr); */
/*  */
/*     auto rows = std::views::iota(0u, chip8_.SCREEN_HEIGHT); */
/*     auto cols = std::views::iota(0u, chip8_.SCREEN_WIDTH); */
/*  */
/*     SDL_SetRenderDrawColor(renderer_, pixel_red, pixel_green, pixel_blue, 0); */
/*     for (const auto row : rows) { */
/*         for (const auto col : cols) { */
/*             auto screen_ind = row_col_to_screen_index(row, col); */
/*             if (screen[screen_ind]) { */
/*                 SDL_Rect rect = {.x = static_cast<int>(col * screen_scale_), */
/*                                  .y = static_cast<int>(row * screen_scale_), */
/*                                  .w = screen_scale_, */
/*                                  .h = screen_scale_}; */
/*                 SDL_RenderFillRect(renderer_, &rect); */
/*             } */
/*         } */
/*     } */
/*  */
/*     SDL_RenderPresent(renderer_); */
}

void Emu::step() {
    auto opcode = chip8_.fetch();
    spdlog::debug("opcode = {:x}", opcode);
    chip8_.execute(opcode);
    render();
}

void Emu::cycle_forward(u8 cycles_remaining) {
    const auto cycles = std::views::iota(0u, cycles_remaining);
    for (const auto c : cycles) {
        chip8_.cycle();
    }
}

/// return the number of chip8 instructions executed
u8 Emu::handle_event(const SDL_Event &event) {
    u8 instructions_executed = 0;

    // first pass event to ImGui
    ImGui_ImplSDL2_ProcessEvent(&event);

    switch (event.type) {
    case SDL_QUIT:
        spdlog::debug("Received SDL_QUIT, exiting.");
        running_ = false;
        break;

    case SDL_KEYDOWN:
        Uint8 const *keys = SDL_GetKeyboardState(nullptr);

        if (chip8_paused_ && keys[SDL_SCANCODE_N] == 1) {
            spdlog::debug("Keydown event: N");
            chip8_.cycle();
            instructions_executed++;
        } else if (chip8_paused_ && keys[SDL_SCANCODE_R] == 1) {
            spdlog::debug("Keydown event: R");
            chip8_paused_ = false;
        } else if (keys[SDL_SCANCODE_P] == 1) {
            spdlog::debug("Keydown event: P");
            chip8_paused_ = true;
        }
        break;
    }

    return instructions_executed;
}

void Emu::run() {
    u8 instructions_executed = 0;
    u8 frames_rendered = 0;
    u32 last_time = 0;
    u32 current_time = 0;

    while (running_) {
        while (SDL_PollEvent(&event_) > 0) {
            instructions_executed += handle_event(event_);
        }

        if (instructions_executed == 10) {
            instructions_executed = 0;
            render();
            frames_rendered++;
        }

        if (!chip8_paused_) {
            cycle_forward(instructions_per_frame_ - instructions_executed);
            instructions_executed = instructions_per_frame_;
        }

        current_time = SDL_GetTicks();
        if (current_time > last_time + 1000) {
            spdlog::debug("Frames over last second = {}", frames_rendered);
            frames_rendered = 0;
            last_time = current_time;
        }
    }
}

void Emu::load_rom(const Rom &rom) {
    chip8_.load_rom(rom);
}


Rom read_rom_file(const std::string_view &path) {
    namespace fs = std::filesystem;
    fs::path file_path{path};

    if (!fs::is_regular_file(file_path)) {
        spdlog::error("Rom File: {} is not a regular file", file_path.string());
        return std::vector<u8>{};
    }

    // stackoverflow post:
    // https://stackoverflow.com/questions/15138353/how-to-read-a-binary-file-into-a-vector-of-unsigned-chars
    std::ifstream rom{file_path, std::ios::binary | std::ios::ate};
    // don't skip whitespace
    rom.unsetf(std::ios::skipws);
    // get size
    rom.seekg(0, std::ios::end);
    const auto size = rom.tellg();

    if (size < 0) {
        spdlog::error("Rom File: {} has size less than zero", file_path.string());
        return Rom{0};
    }
    spdlog::debug("Rom {} size: {}", path, size);

    std::vector<u8> rom_data(static_cast<size_t>(size));
    // return to beginning and copy
    rom.seekg(0, std::ios::beg);
    std::copy(std::istream_iterator<u8>(rom), std::istream_iterator<u8>(),
              std::begin(rom_data));

    spdlog::debug("Printing rom_data");
    for (auto i = 0; i < size; ++i) {
        spdlog::debug("byte {}: {}", i, rom_data[i]);
    }

    return Rom{std::move(rom_data)};
}
