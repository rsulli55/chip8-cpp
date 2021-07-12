#include "SDL.h"
#include "spdlog/spdlog.h"

#include "chip8.h"
#include "common.h"
#include <SDL_pixels.h>
#include <SDL_render.h>
#include <cstdlib>
#include <exception>
#include <filesystem>
#include <string_view>
#include <fstream>

class Emu {

  private:
    SDL_Window *window_ = nullptr;
    SDL_Renderer *renderer_ = nullptr;

    Chip8 &chip8_;
    u8 screen_scale_;
    u32 screen_width_;
    u32 screen_height_;
    const u8 background_red = 0x0F;
    const u8 background_green = 0x0F;
    const u8 background_blue = 0xFF;

    const u8 pixel_red = 0xFF;
    const u8 pixel_green = 0x00;
    const u8 pixel_blue = 0x0F;

    // constants
    const char *WINDOW_NAME = "Chip8-cpp";

    u32 init_SDL() {
        if (SDL_Init(SDL_INIT_VIDEO) < 0) {
            spdlog::error("Could not initialize SDL\nError: %s\n",
                          SDL_GetError());
            return 10;
        }

        else {
            window_ = SDL_CreateWindow(WINDOW_NAME, SDL_WINDOWPOS_UNDEFINED,
                                       SDL_WINDOWPOS_UNDEFINED, screen_width_,
                                       screen_height_, SDL_WINDOW_SHOWN);

            if (!window_) {
                spdlog::error("Window could not be created!\nError: %s\n",
                              SDL_GetError());
                return 20;
            } else {
                renderer_ = SDL_CreateRenderer(window_, -1, SDL_RENDERER_ACCELERATED);
                return 0;
            }
        }
    }

  public:
    Emu(Chip8 &chip8, u8 screen_scale)
        : chip8_{chip8}, screen_scale_{screen_scale},
          screen_width_{chip8.SCREEN_WIDTH * screen_scale},
          screen_height_{chip8.SCREEN_HEIGHT * screen_scale} {
        u32 ec = init_SDL();
        // terminate if SDL does not load correctly
        if (ec != 0) {
            std::abort();
        }
    }

    ~Emu() {
        SDL_DestroyRenderer(renderer_);
        SDL_DestroyWindow(window_);
        SDL_Quit();
    }

    void render() {
        SDL_RenderClear(renderer_);
        const auto &screen = chip8_.screen();
        SDL_SetRenderDrawColor(renderer_, background_red, background_green, background_blue, 0);
        SDL_RenderFillRect(renderer_, nullptr);

        auto rows = std::views::iota(0u, chip8_.SCREEN_HEIGHT);
        auto cols = std::views::iota(0u, chip8_.SCREEN_WIDTH);

        SDL_SetRenderDrawColor(renderer_, pixel_red, pixel_green, pixel_blue, 0);
        for (const auto row : rows) {
            for (const auto col : cols) {
                if (screen[row][col]) {
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

    void step() {
        auto opcode = chip8_.fetch();
        spdlog::debug("opcode = {:x}", opcode);
        chip8_.execute(opcode);
        render();
    }

    std::vector<u8> load_rom_file(const std::string_view &path) {
        namespace fs = std::filesystem;
        fs::path file_path{path};

        if (!fs::is_regular_file(file_path)) {
            spdlog::error("Rom File: {} is not a regular file",
                          file_path.string());
            return std::vector<u8>{};
        }

        // stackoverflow post:
        // https://stackoverflow.com/questions/15138353/how-to-read-a-binary-file-into-a-vector-of-unsigned-chars
        std::ifstream rom {file_path, std::ios::binary};
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

        return rom_data;
    }
};
