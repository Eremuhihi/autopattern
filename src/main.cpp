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

constexpr int WINDOW_WIDTH  = 640;
constexpr int WINDOW_HEIGHT = 480;

static bool fillPlatformData(SDL_Window* window, bgfx::PlatformData& outPd)
{
    SDL_SysWMinfo wmi;
    SDL_VERSION(&wmi.version);
    if (!SDL_GetWindowWMInfo(window, &wmi)) {
        std::cerr << "SDL_GetWindowWMInfo failed: " << SDL_GetError() << "\n";
        return false;
    }
    outPd = {};
#if defined(_WIN32)
    outPd.nwh = wmi.info.win.window;
    outPd.ndt = nullptr;
    return true;
#elif defined(__APPLE__)
    outPd.nwh = wmi.info.cocoa.window;
    outPd.ndt = nullptr;
    return true;
#else
    switch (wmi.subsystem) {
    case SDL_SYSWM_X11:
        outPd.ndt = wmi.info.x11.display;
        outPd.nwh = (void*)(uintptr_t)wmi.info.x11.window;
        return true;
    case SDL_SYSWM_WAYLAND:
        outPd.ndt = wmi.info.wl.display;
        outPd.nwh = wmi.info.wl.surface;
        return true;
    default:
        std::cerr << "Unsupported SDL subsystem\n";
        return false;
    }
#endif
}

static const bgfx::Memory* loadFile(const char* path)
{
    std::ifstream ifs(path, std::ios::binary);
    if (!ifs) return nullptr;
    std::vector<char> data((std::istreambuf_iterator<char>(ifs)),
                            std::istreambuf_iterator<char>());
    const bgfx::Memory* mem = bgfx::copy(data.data(), (uint32_t)data.size());
    return mem;
}

static bgfx::ShaderHandle loadShader(const char* path)
{
    const bgfx::Memory* mem = loadFile(path);
    if (!mem) {
        std::cerr << "Failed to load shader: " << path << "\n";
        return BGFX_INVALID_HANDLE;
    }
    return bgfx::createShader(mem);
}

static bgfx::ProgramHandle loadProgram(const char* vsBin, const char* fsBin)
{
    bgfx::ShaderHandle vsh = loadShader(vsBin);
    bgfx::ShaderHandle fsh = loadShader(fsBin);
    if (!bgfx::isValid(vsh) || !bgfx::isValid(fsh)) {
        if (bgfx::isValid(vsh)) bgfx::destroy(vsh);
        if (bgfx::isValid(fsh)) bgfx::destroy(fsh);
        return BGFX_INVALID_HANDLE;
    }
    return bgfx::createProgram(vsh, fsh, true);
}

struct PosUv { float x,y,z, u,v; };

int main()
{
    SDL_Init(SDL_INIT_VIDEO);

    int ww = WINDOW_WIDTH, wh = WINDOW_HEIGHT;
    SDL_Window* window = SDL_CreateWindow(
        "noise",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        ww, wh,
        SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE
    );
    if (!window) {
        std::cerr << "SDL_CreateWindow failed: " << SDL_GetError() << "\n";
        return 1;
    }

    bgfx::PlatformData pd{};
    if (!fillPlatformData(window, pd)) {
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }
    bgfx::setPlatformData(pd);

    // === OpenGL を明示（CMake は GLSL 150 を出力） ===
    bgfx::Init init{};
#if defined(_WIN32)
    init.type = bgfx::RendererType::Direct3D11;
#elif defined(__APPLE__)
    init.type = bgfx::RendererType::Metal;
#else
    init.type = bgfx::RendererType::OpenGL;
#endif
    init.platformData = pd;
    init.resolution.width  = (uint32_t)ww;
    init.resolution.height = (uint32_t)wh;
    init.resolution.reset  = BGFX_RESET_VSYNC;
    if (!bgfx::init(init)) {
        std::cerr << "bgfx::init failed\n";
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    bgfx::setViewClear(0, BGFX_CLEAR_COLOR, 0xff0000ff);
    bgfx::setViewMode(0, bgfx::ViewMode::Sequential);
    bgfx::setViewRect(0, 0, 0, (uint16_t)ww, (uint16_t)wh);

    // プログラム（bin は実行ファイルの横の shaders/ にある想定）
    bgfx::ProgramHandle program = loadProgram(
        "shaders/vs_fullscreen.bin",
        "shaders/fs_texture.bin"
    );
    if (!bgfx::isValid(program)) {
        std::cerr << "Failed to load shader program.\n";
        bgfx::shutdown(); SDL_DestroyWindow(window); SDL_Quit();
        return 1;
    }

    // uniform
    bgfx::UniformHandle u_tex0 = bgfx::createUniform("u_tex0", bgfx::UniformType::Sampler);

    // フルスクリーン四角形
    bgfx::VertexLayout layout;
    layout.begin()
        .add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
        .add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float)
    .end();

    const PosUv quadVerts[] = {
        {-1.f,-1.f,0.f, 0.f,1.f},
        { 1.f,-1.f,0.f, 1.f,1.f},
        { 1.f, 1.f,0.f, 1.f,0.f},
        {-1.f, 1.f,0.f, 0.f,0.f},
    };
    const uint16_t quadIdx[] = { 0,1,2, 0,2,3 };

    bgfx::VertexBufferHandle vbh = bgfx::createVertexBuffer(
        bgfx::copy(quadVerts, sizeof(quadVerts)), layout);
    bgfx::IndexBufferHandle ibh = bgfx::createIndexBuffer(
        bgfx::copy(quadIdx, sizeof(quadIdx)));

    // 1枚のテクスチャを毎フレーム更新
    bgfx::TextureHandle tex = bgfx::createTexture2D(
        (uint16_t)ww, (uint16_t)wh, false, 1,
        bgfx::TextureFormat::BGRA8, 0);

    std::vector<uint32_t> pixels((size_t)ww * (size_t)wh);
    std::srand((unsigned)std::time(nullptr));

    bool running = true;
    SDL_Event event;
    while (running) {
        bool resized = false;

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) running = false;
            if (event.type == SDL_WINDOWEVENT &&
               (event.window.event == SDL_WINDOWEVENT_RESIZED ||
                event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED)) {
                resized = true;
            }
        }

        if (resized) {
            SDL_GetWindowSize(window, &ww, &wh);
            if (ww < 1) ww = 1;
            if (wh < 1) wh = 1;

            bgfx::reset((uint32_t)ww, (uint32_t)wh, BGFX_RESET_VSYNC);
            bgfx::setViewRect(0, 0, 0, (uint16_t)ww, (uint16_t)wh);

            if (bgfx::isValid(tex)) bgfx::destroy(tex);
            tex = bgfx::createTexture2D((uint16_t)ww, (uint16_t)wh, false, 1,
                                        bgfx::TextureFormat::BGRA8, 0);
            pixels.assign((size_t)ww * (size_t)wh, 0);
        }

        // 乱数画像を埋める（白/黒）
        for (size_t i = 0, n = pixels.size(); i < n; ++i) {
            bool white = (std::rand() & 1) == 0;
            pixels[i] = white ? 0xFFFFFFFFu : 0xFF000000u;
        }

        // テクスチャ更新
        const bgfx::Memory* mem =
            bgfx::copy(pixels.data(), (uint32_t)(pixels.size() * sizeof(uint32_t)));
        bgfx::updateTexture2D(tex, 0, 0, 0, 0, (uint16_t)ww, (uint16_t)wh, mem);

        // 描画
        bgfx::touch(0);
        bgfx::setTexture(0, u_tex0, tex);
        bgfx::setVertexBuffer(0, vbh);
        bgfx::setIndexBuffer(ibh);
        bgfx::setState(BGFX_STATE_WRITE_RGB | BGFX_STATE_WRITE_A);
        bgfx::submit(0, program);

        bgfx::frame();
        //std::this_thread::sleep_for(std::chrono::milliseconds(16));
    }

    if (bgfx::isValid(tex)) bgfx::destroy(tex);
    if (bgfx::isValid(ibh)) bgfx::destroy(ibh);
    if (bgfx::isValid(vbh)) bgfx::destroy(vbh);
    if (bgfx::isValid(u_tex0)) bgfx::destroy(u_tex0);
    if (bgfx::isValid(program)) bgfx::destroy(program);

    bgfx::shutdown();
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}

