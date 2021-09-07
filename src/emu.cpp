#include "emu.h"
#include "SDL.h"
#include "glbinding/glbinding.h"
#include "imgui.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_sdl.h"

Emu::Emu(u8 screen_scale, State state)
    : window_{screen_scale},
      shader_{vert_shader_path, frag_shader_path},
      renderer_{shader_} {
    // use shader program
    glUseProgram(shader_.id());
}

void Emu::render() {
    const auto &screen = chip8_.screen();
    renderer_.render(screen);
    window_.swap_window();
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

    case SDL_WINDOWEVENT:
        if (event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
            spdlog::debug("SDL_WINDOWEVENT: SIZE_CHANGED to ({}, {})", 
                    event.window.data1, event.window.data2);
            window_.resize();
        }
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

        if (!chip8_paused_) {
            cycle_forward(instructions_per_frame_ - instructions_executed);
            instructions_executed = instructions_per_frame_;
        }

        if (instructions_executed >= 10) {
            instructions_executed = 0;
        }

        if (instructions_executed == 0) {
            render();
            frames_rendered++;
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


Rom read_rom_file(std::string_view path) {
    namespace fs = std::filesystem;
    fs::path file_path{path};

    if (!fs::is_regular_file(file_path)) {
        spdlog::error("Rom File: {} is not a regular file", file_path.string());
        return Rom{std::vector<u8>{}};
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
