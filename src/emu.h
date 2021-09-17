#include "SDL.h"
#include "spdlog/spdlog.h"

#include <SDL_pixels.h>
#include <SDL_render.h>
#include <cstdlib>
#include <exception>
#include <filesystem>
#include <fstream>
#include <string_view>
#include <map>

#include "chip8.h"
#include "window.h"
#include "rom.h"
#include "common.h"
#include "renderer.h"
#include "instruction.h"


class Emu {

  public:
    enum class State { Debug, Pause, Run };

  private:
    SDL_Event event_;


    Chip8 chip8_;

    // TODO: move some of these to a Settings struct
    /* u8 screen_scale_; */
    /* u32 window_width_; */
    /* u32 window_height_; */
    u32 frames_per_second_ = 60;
    u32 instructions_per_frame_ = 10;

    State state_;
    bool running_ = true;
    bool chip8_paused_ = true;

    Window window_;
    Renderer renderer_;

    std::map<InstructionType, std::string> instruction_table_;

  public:
    Emu(u8 screen_scale, State state);

    void load_rom(const Rom& rom);
    void run();
    void cycle_forward(u8 cycles_remaining);
    void render();
    void step();
    auto handle_event(const SDL_Event &event) -> u8;

    auto instruction_table() -> const std::map<InstructionType, std::string>& {
        return instruction_table_;
    }

    static constexpr std::string_view instruction_table_path = "instruction_table.csv";

  private:
};

auto read_instruction_table(std::string_view path) -> std::map<InstructionType, std::string>;
auto read_rom_file(std::string_view path) -> Rom;
