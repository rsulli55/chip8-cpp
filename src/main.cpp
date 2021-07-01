#include <iostream>

#include "SDL.h"
#include "spdlog/spdlog.h"

int main(int argc, char *argv[]) {
    std::cout << "Hello world!\n";
    spdlog::info("Hello from spdlog\n");

    SDL_Window *window = nullptr;
    SDL_Surface *surface = nullptr;

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        spdlog::error("Could not initialize SDL\nError: %s\n", SDL_GetError());
    }

    else {
        window = SDL_CreateWindow("SDL Window", SDL_WINDOWPOS_UNDEFINED,
                                  SDL_WINDOWPOS_UNDEFINED, 640, 480,
                                  SDL_WINDOW_SHOWN);

        if (!window)
            spdlog::error("Window could not be created!\nError: %s\n",
                          SDL_GetError());
        else {
            surface = SDL_GetWindowSurface(window);
            SDL_FillRect(surface, nullptr,
                         SDL_MapRGB(surface->format, 0xFF, 0xFF, 0xFF));

            SDL_UpdateWindowSurface(window);

            SDL_Delay(2000);
        }
    }

    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
