#include <SDL.h>
#include <SDL_syswm.h>
#include <bgfx/bgfx.h>
#include <bgfx/platform.h>
#include <bx/math.h>
#include <cstdio>

#if defined(_WIN32)
  #define SDL_SYSWM_TYPE  SDL_SYSWM_WINDOWS
#elif defined(__APPLE__)
  #define SDL_SYSWM_TYPE  SDL_SYSWM_COCOA
#else
  #define SDL_SYSWM_TYPE  SDL_SYSWM_X11  // WSLg/多くのLinuxでOK（WaylandでもSDLが面倒みる）
#endif

static bgfx::PlatformData s_pd{};

static void set_platform_data_from_sdl(SDL_Window* window)
{
  SDL_SysWMinfo wmi;
  SDL_VERSION(&wmi.version);
  if (!SDL_GetWindowWMInfo(window, &wmi)) {
    std::fprintf(stderr, "SDL_GetWindowWMInfo failed: %s\n", SDL_GetError());
    return;
  }

  // すべて nullptr で初期化
  s_pd.ndt = nullptr; s_pd.nwh = nullptr; s_pd.context = nullptr; s_pd.backBuffer = nullptr; s_pd.backBufferDS = nullptr;

#if defined(_WIN32)
  s_pd.nwh = wmi.info.win.window;
#elif defined(__APPLE__)
  s_pd.nwh = wmi.info.cocoa.window;
#else
  // X11 / Wayland など。SDL が抽象化してくれるが bgfx にはハンドルが必要
  // X11 の場合:
  if (wmi.subsystem == SDL_SYSWM_X11) {
    s_pd.ndt = wmi.info.x11.display;
    s_pd.nwh = (void*)(uintptr_t)wmi.info.x11.window;
  }
  // Wayland の場合（SDL 2.0.22+）
  if (wmi.subsystem == SDL_SYSWM_WAYLAND) {
    s_pd.ndt = wmi.info.wl.display;
    s_pd.nwh = wmi.info.wl.surface;
  }
#endif
  bgfx::setPlatformData(s_pd);
}

int main(int argc, char** argv)
{
  (void)argc; (void)argv;
  if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_TIMER) != 0) {
    std::fprintf(stderr, "SDL_Init failed: %s\n", SDL_GetError());
    return 1;
  }

  const int width = 1280, height = 720;
  SDL_Window* window = SDL_CreateWindow("cellauto-bgfx",
      SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
      width, height, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
  if (!window) {
    std::fprintf(stderr, "SDL_CreateWindow failed: %s\n", SDL_GetError());
    return 1;
  }

  // bgfx: ウィンドウハンドルを渡す
  set_platform_data_from_sdl(window);

  // レンダラは自動選択（Windows/D3D, Linux/Vulkan/GL, macOS/Metal）
  bgfx::Init init;
  init.type = bgfx::RendererType::Count; // Auto
  init.vendorId = BGFX_PCI_ID_NONE;
  init.resolution.width  = width;
  init.resolution.height = height;
  init.resolution.reset  = BGFX_RESET_VSYNC;

  if (!bgfx::init(init)) {
    std::fprintf(stderr, "bgfx::init failed\n");
    return 1;
  }

  // ビューポート
  bgfx::setViewClear(0, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x223344ff, 1.0f, 0);
  bgfx::setViewRect(0, 0, 0, uint16_t(width), uint16_t(height));

  bool quit = false;
  bool paused = false;
  uint64_t lastTicks = SDL_GetTicks64();
  float t = 0.0f;

  while (!quit) {
    SDL_Event ev;
    while (SDL_PollEvent(&ev)) {
      if (ev.type == SDL_QUIT) quit = true;
      if (ev.type == SDL_KEYDOWN) {
        if (ev.key.keysym.sym == SDLK_ESCAPE) quit = true;
        if (ev.key.keysym.sym == SDLK_SPACE)  paused = !paused;
      }
      if (ev.type == SDL_WINDOWEVENT &&
          (ev.window.event == SDL_WINDOWEVENT_RESIZED ||
           ev.window.event == SDL_WINDOWEVENT_SIZE_CHANGED))
      {
        int w, h; SDL_GetWindowSize(window, &w, &h);
        bgfx::reset((uint32_t)w, (uint32_t)h, BGFX_RESET_VSYNC);
        bgfx::setViewRect(0, 0, 0, (uint16_t)w, (uint16_t)h);
      }
    }

    // 背景色をゆっくり変える（シェーダ不要の可視フィードバック）
    uint64_t now = SDL_GetTicks64();
    float dt = float(now - lastTicks)*0.001f;
    lastTicks = now;
    if (!paused) t += dt;

    uint8_t r = uint8_t((std::sin(t*0.7f)*0.5f + 0.5f)*255);
    uint8_t g = uint8_t((std::sin(t*0.9f + 2.0f)*0.5f + 0.5f)*255);
    uint8_t b = uint8_t((std::sin(t*1.3f + 4.0f)*0.5f + 0.5f)*255);
    uint32_t abgr = 0xff000000 | (uint32_t(r)      ) | (uint32_t(g)<<8) | (uint32_t(b)<<16);
    bgfx::setViewClear(0, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, abgr, 1.0f, 0);

    bgfx::touch(0); // 何も描かなくても view 0 を“生かす”

    // デバッグテキスト（シェーダなしで文字表示できる）
    bgfx::dbgTextClear();
    bgfx::dbgTextPrintf(0, 0, 0x0f, "cellauto-bgfx : bgfx + SDL2 minimal");
    bgfx::dbgTextPrintf(0, 1, 0x0a, "[Space] pause=%s, [Esc] quit", paused ? "true" : "false");

    bgfx::frame();
  }

  bgfx::shutdown();
  SDL_DestroyWindow(window);
  SDL_Quit();
  return 0;
}

