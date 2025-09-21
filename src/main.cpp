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

#include <panel_set.hpp>
#include <noise.hpp>
#include <bad_noise.hpp>

constexpr int WINDOW_WIDTH  = 1280;
constexpr int WINDOW_HEIGHT = 720;
//constexpr int WINDOW_WIDTH  = 256;
//constexpr int WINDOW_HEIGHT = 256;

int main()
{
    // Initialize SDL for video
    SDL_Init(SDL_INIT_VIDEO);

    // Create main application window
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

    // Setup bgfx platform data from SDL window
    bgfx::PlatformData pd{};
    SDL_SysWMinfo wmi;
    SDL_VERSION(&wmi.version);
    if (!SDL_GetWindowWMInfo(window, &wmi)) {
        std::cerr << "SDL_GetWindowWMInfo failed: " << SDL_GetError() << "\n";
        SDL_DestroyWindow(window);
        SDL_Quit();
    }
    pd = {};
#if defined(_WIN32)
    pd.nwh = wmi.info.win.window;
    pd.ndt = nullptr;
#elif defined(__APPLE__)
    pd.nwh = wmi.info.cocoa.window;
    pd.ndt = nullptr;
#else
    switch (wmi.subsystem) {
    case SDL_SYSWM_X11:
        pd.ndt = wmi.info.x11.display;
        pd.nwh = (void*)(uintptr_t)wmi.info.x11.window;
        break;
    case SDL_SYSWM_WAYLAND:
        pd.ndt = wmi.info.wl.display;
        pd.nwh = wmi.info.wl.surface;
        break;
    default:
        std::cerr << "Unsupported SDL subsystem\n";
        SDL_DestroyWindow(window);
        SDL_Quit();
        break;
    }
#endif

    bgfx::setPlatformData(pd);

    // Initialize bgfx (choose renderer by platform)
    bgfx::Init init{};
#if defined(_WIN32)
    init.type = bgfx::RendererType::Direct3D11;
#elif defined(__APPLE__)
    init.type = bgfx::RendererType::Metal;
#else
    init.type = bgfx::RendererType::OpenGL;
#endif
    init.platformData = pd;
    {
        int width = 0, height = 0; 
        SDL_GetWindowSize(window, &width, &height);
        init.resolution.width  = static_cast<uint32_t>(width);
        init.resolution.height = static_cast<uint32_t>(height);
    }
    init.resolution.reset  = BGFX_RESET_VSYNC;
    if (!bgfx::init(init)) {
        std::cerr << "bgfx::init failed\n";
        SDL_DestroyWindow(window);
        SDL_Quit();
    }

    // Create panel manager
    atpt::PanelSet panels(window);

    // Add two panels (Noise and BadNoise)
    atpt::Noise& noise        = panels.createPanel<atpt::Noise>(window, 19937);
    atpt::BadNoise& bad_noise = panels.createPanel<atpt::BadNoise>(window);

    bool running = true;
    SDL_Event event;
    
    // Set window title to current panel name
    SDL_SetWindowTitle(window, panels.name().c_str());

    // Main loop
    while (running) {
        bool resized = false;

        // Handle events
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) running = false;
            panels.event(event);
        }

        // Draw active panel
        panels.draw();
    }

    // Cleanup
    panels.destroy();
    bgfx::shutdown();
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
