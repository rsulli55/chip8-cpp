#include <ranges>
#include <numeric>
#include <algorithm>

#include "SDL.h"
#include "glbinding/glbinding.h"
#include "imgui.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_sdl.h"

#include "emu.h"
#include "instruction.h"

Emu::Emu(Chip8 chip8, u8 screen_scale)
    : chip8_{std::move(chip8)},
    window_{screen_scale, renderer_}, 
    renderer_{} {}

void Emu::render() {
    auto past_ops = build_past_opcodes(chip8s_, MAX_PAST_OPS);
    window_.render(past_ops, chip8_, FUTURE_OPS);
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
        if (keys[SDL_SCANCODE_P] == 1) {
            chip8_paused_ = !chip8_paused_;
        }
        else if (chip8_paused_ && keys[SDL_SCANCODE_N] == 1) {
            push_chip8();
            chip8_.cycle();
            instructions_executed++;
        } else if (keys[SDL_SCANCODE_R] == 1 && chip8s_.size() > 0) {
            chip8_paused_ = true;
            pop_chip8();
            // we popped a full frame of instructions
            instructions_executed = instructions_per_frame_;
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
            instructions_executed = handle_event(event_);
        }

        // if not paused we would like to make sure we execute
        // `instructions_per_frame_` instructions before updating display
        if (!chip8_paused_) {
            push_chip8();
            cycle_forward(instructions_per_frame_ - instructions_executed);
            instructions_executed = 0;
        }
        // if we are paused then after we need to reset instructions_executed
        // once enough instructions have been stepped through
        else if (instructions_executed >= instructions_per_frame_) {
            instructions_executed = 0;
        }

        // render a frame
        render();

        /* frames_rendered++; */
        /* current_time = SDL_GetTicks(); */
        /* if (current_time > last_time + 1000) { */
        /*     spdlog::debug("Frames over last second = {}", frames_rendered); */
        /*     frames_rendered = 0; */
        /*     last_time = current_time; */
        /* } */
    }
}

void Emu::load_rom(const Rom &rom) { chip8_.load_rom(rom); }

void inline Emu::push_chip8() {
    chip8s_.emplace_front(chip8_);
    if (chip8s_.size() > CHIP8_HISTORY_AMOUNT) chip8s_.pop_back();
}

void inline Emu::pop_chip8() {
    chip8_ = chip8s_.front();
    chip8s_.pop_front();
}

auto read_rom_file(std::string_view path) -> Rom {
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
        spdlog::error("Rom File: {} has size less than zero",
                      file_path.string());
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

auto build_past_opcodes(const std::deque<Chip8>& chip8s, u32 max_ops) -> std::vector<u16> {
    const auto past_to_show = std::min(max_ops, static_cast<u32>(chip8s.size()));
    std::vector<u16> opcodes;
    opcodes.reserve(past_to_show);

    auto view = std::ranges::take_view{chip8s, past_to_show} |
        std::views::transform([](const Chip8& chip8) { 
                    return chip8.fetch_with_pc(chip8.pc());
                    }) |
        std::views::reverse;

    std::ranges::copy(view, std::back_inserter(opcodes));

    return opcodes;
}
