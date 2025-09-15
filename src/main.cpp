#include <SDL2/SDL.h>
#include <SDL2/SDL_syswm.h>
#include <bgfx/bgfx.h>
#include <bgfx/platform.h>
#include <cstdlib>
#include <ctime>
#include <thread>
#include <vector>
#include <iostream>
#include <fstream>

#include <noise.hpp>

constexpr int WINDOW_WIDTH  = 256;
constexpr int WINDOW_HEIGHT = 256;

int main()
{
    SDL_Init(SDL_INIT_VIDEO);

    int ww = WINDOW_WIDTH, wh = WINDOW_HEIGHT;
    SDL_Window* window = SDL_CreateWindow(
        "autopattern",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        ww, wh,
        SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE
    );
    if (!window) {
        std::cerr << "SDL_CreateWindow failed: " << SDL_GetError() << "\n";
        return 1;
    }

    atpt::Noise noise(window, 19937);   

    bool running = true;
    SDL_Event event;
    
    SDL_SetWindowTitle(window, noise.name().c_str());
    while (running) {
        bool resized = false;

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) running = false;
            noise.event(window, event);
        }
        noise.draw();
    }

    noise.destroy();

    bgfx::shutdown();
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}

