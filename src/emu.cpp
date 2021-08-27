#include "emu.h"
#include <SDL_render.h>

u32 Emu::init_SDL() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        spdlog::error("Could not initialize SDL\nError: %s\n", SDL_GetError());
        return 10;
    }

    window_ = SDL_CreateWindow(WINDOW_NAME, SDL_WINDOWPOS_UNDEFINED,
                               SDL_WINDOWPOS_UNDEFINED, screen_width_,
                               screen_height_, SDL_WINDOW_SHOWN);

    if (!window_) {
        spdlog::error("Window could not be created!\nError: %s\n",
                      SDL_GetError());
        return 20;
    }
    renderer_ = SDL_CreateRenderer(
        window_, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer_) {
        spdlog::error("Renderer could not be created!\nError: %s\n",
                      SDL_GetError());
        return 30;
    }

    return 0;
}
Emu::Emu(u8 screen_scale, State state)
    : screen_scale_{screen_scale}, state_{state},
      screen_width_{chip8_.SCREEN_WIDTH * screen_scale},
      screen_height_{chip8_.SCREEN_HEIGHT * screen_scale} {
    u32 ec = init_SDL();
    // terminate if SDL does not load correctly
    if (ec != 0) {
        std::abort();
    }
}

Emu::~Emu() {
    SDL_DestroyRenderer(renderer_);
    SDL_DestroyWindow(window_);
    SDL_Quit();
}

void Emu::render() {
    SDL_RenderClear(renderer_);
    const auto &screen = chip8_.screen();
    SDL_SetRenderDrawColor(renderer_, background_red, background_green,
                           background_blue, 0);
    SDL_RenderFillRect(renderer_, nullptr);

    auto rows = std::views::iota(0u, chip8_.SCREEN_HEIGHT);
    auto cols = std::views::iota(0u, chip8_.SCREEN_WIDTH);

    SDL_SetRenderDrawColor(renderer_, pixel_red, pixel_green, pixel_blue, 0);
    for (const auto row : rows) {
        for (const auto col : cols) {
            auto screen_ind = row_col_to_screen_index(row, col);
            if (screen[screen_ind]) {
                SDL_Rect rect = {.x = static_cast<int>(col * screen_scale_),
                                 .y = static_cast<int>(row * screen_scale_),
                                 .w = screen_scale_,
                                 .h = screen_scale_};
                SDL_RenderFillRect(renderer_, &rect);
            }
        }
    }

    SDL_RenderPresent(renderer_);
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

std::vector<u8> Emu::load_rom_file(const std::string_view &path) {
    namespace fs = std::filesystem;
    fs::path file_path{path};

    if (!fs::is_regular_file(file_path)) {
        spdlog::error("Rom File: {} is not a regular file", file_path.string());
        return std::vector<u8>{};
    }

    // stackoverflow post:
    // https://stackoverflow.com/questions/15138353/how-to-read-a-binary-file-into-a-vector-of-unsigned-chars
    std::ifstream rom{file_path, std::ios::binary};
    rom.unsetf(std::ios::skipws);
    // get size
    rom.seekg(0, std::ios::end);
    const auto size = rom.tellg();
    rom.seekg(0, std::ios::beg);

    spdlog::debug("Rom {} size: {}", path, size);

    // make vector and insert
    std::vector<u8> rom_data(size);
    std::copy(std::istream_iterator<u8>(rom), std::istream_iterator<u8>(),
              std::begin(rom_data));

    spdlog::debug("Printing rom_data");
    for (auto i = 0; i < size; ++i) {
        spdlog::debug("byte {}: {}", i, rom_data[i]);
    }

    // TODO: Refactor?
    chip8_.load_rom(rom_data);
    chip8_.set_debug_level(spdlog::level::debug);
    return rom_data;
}
