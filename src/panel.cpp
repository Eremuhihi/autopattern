#include <panel.hpp>
#include <iostream>
#include <fstream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_syswm.h>
#include <bgfx/bgfx.h>
#include <bgfx/platform.h>

namespace atpt{

    //+    Member Function    +//
    auto Panel::_getWidthFromWindowHandle (void)
        -> int
    {
        int w, h;
        SDL_GetWindowSize(_wh, &w, &h);
        return w;
    }

    auto Panel::_getHeightFromWindowHandle (void)
        -> int
    {
        int w, h;
        SDL_GetWindowSize(_wh, &w, &h);
        return h;
    }

    //_ Consructor
    Panel::Panel (const std::string& name_, SDL_Window* wdh_, const std::filesystem::path& fs_path_)
        : _name          { name_ }
        , _wh            { wdh_}
        , _width         { _getWidthFromWindowHandle() }
        , _height        { _getHeightFromWindowHandle() } 
        , _ph            { }
        , _vbh           { }
        , _ibh           { }    
    {
        bgfx::setViewClear(0, BGFX_CLEAR_COLOR, 0xff0000ff);
        bgfx::setViewMode(0, bgfx::ViewMode::Sequential);
        bgfx::setViewRect(0, 0, 0, static_cast<uint16_t>(_width), static_cast<uint16_t>(_height));


        // Vertex Shader
        _vsh = BGFX_INVALID_HANDLE;
        {
            std::ifstream ifs("shaders/vs_fullscreen.bin", std::ios::binary);
            if (!ifs) {
                std::cerr << "Failed to load shader: shaders/vs_fullscreen.bin\n";
                bgfx::shutdown(); SDL_DestroyWindow(wdh_); SDL_Quit();
            }
        
            std::vector<char> data((std::istreambuf_iterator<char>(ifs)),
                                   std::istreambuf_iterator<char>());
            const bgfx::Memory* mem = bgfx::copy(data.data(), (uint32_t)data.size());
            _vsh = bgfx::createShader(mem);
            if (!bgfx::isValid(_vsh)) {
                std::cerr << "Failed to create vertex shader.\n";
                bgfx::shutdown(); SDL_DestroyWindow(wdh_); SDL_Quit();
            }
        }

        // Fragment Shader
        _fsh = BGFX_INVALID_HANDLE;
        {
            std::ifstream ifs(fs_path_, std::ios::binary);
            if (!ifs) {
                std::cerr << "Failed to load shader: shaders/fs_texture.bin\n";
                if (bgfx::isValid(_fsh)) bgfx::destroy(_fsh);
                bgfx::shutdown(); SDL_DestroyWindow(wdh_); SDL_Quit();
            }
        
            std::vector<char> data((std::istreambuf_iterator<char>(ifs)),
                                   std::istreambuf_iterator<char>());
            const bgfx::Memory* mem = bgfx::copy(data.data(), (uint32_t)data.size());
            _fsh = bgfx::createShader(mem);
            if (!bgfx::isValid(_fsh)) {
                std::cerr << "Failed to create fragment shader.\n";
                if (bgfx::isValid(_fsh)) bgfx::destroy(_fsh);
                bgfx::shutdown(); SDL_DestroyWindow(wdh_); SDL_Quit();
            }
        }

        // Make Program
        _ph = bgfx::createProgram(_vsh, _fsh, true);
        if (!bgfx::isValid(_ph)) {
            std::cerr << "Failed to link shader program.\n";
            if (bgfx::isValid(_vsh)) bgfx::destroy(_vsh);
            if (bgfx::isValid(_fsh)) bgfx::destroy(_fsh);
            bgfx::shutdown(); SDL_DestroyWindow(wdh_); SDL_Quit();
        }

        // フルスクリーン四角形
        bgfx::VertexLayout layout;
        layout.begin()
            .add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
            .add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float)
        .end();

        const struct{float x, y, z, u, v; } quadVerts[] = {
            {-1.f,-1.f,0.f, 0.f,1.f},
            { 1.f,-1.f,0.f, 1.f,1.f},
            { 1.f, 1.f,0.f, 1.f,0.f},
            {-1.f, 1.f,0.f, 0.f,0.f},
        };
        const uint16_t quadIdx[] = { 0,1,2, 0,2,3 };

        _vbh = bgfx::createVertexBuffer(
            bgfx::copy(quadVerts, sizeof(quadVerts)), layout);
        _ibh = bgfx::createIndexBuffer(
            bgfx::copy(quadIdx, sizeof(quadIdx)));

        return;   
    }


    //_ Variable Function
    auto Panel::draw (void)
        -> int
    {
        bgfx::touch(0);
    
        if (int ret = this->_draw()) {
            std::cerr << "setPixcels error " << std::endl;
            return ret;
        }

        bgfx::setVertexBuffer(0, _vbh);
        bgfx::setIndexBuffer(_ibh);
        bgfx::setState(BGFX_STATE_WRITE_RGB | BGFX_STATE_WRITE_A);
        bgfx::submit(0, _ph);

        bgfx::frame();

        return 0;
    }
    
   
    auto Panel::event (const SDL_Event& e_)
        -> int
    {
        if (e_.type == SDL_WINDOWEVENT and
            (
                e_.window.event == SDL_WINDOWEVENT_RESIZED or
                e_.window.event == SDL_WINDOWEVENT_SIZE_CHANGED
            )
        )
        {
            int old_width   = _width;
            int old_height  = _height;
           
            _width =  _getWidthFromWindowHandle();
            _height = _getHeightFromWindowHandle();
            
            bgfx::reset(static_cast<uint32_t>(_width), static_cast<uint32_t>(_height), BGFX_RESET_VSYNC);
            bgfx::setViewRect(0, 0, 0, static_cast<uint16_t>(_width), static_cast<uint16_t>(_height));

            if (int ret = this->_resize(old_width, old_height)) {
                std::cerr << "reize error" << std::endl;
                return ret;
            }
        }else{
            if (int ret = this->_event(e_)){
                std::cerr << "event error" << std::endl;
                return ret;
            };
        }

        return 0;
    }
    
    auto Panel::destroy (void)
        -> int
    {
        this->_destroy();
        if (bgfx::isValid(_ibh)) bgfx::destroy(_ibh);
        if (bgfx::isValid(_vbh)) bgfx::destroy(_vbh);
        if (bgfx::isValid(_fsh)) bgfx::destroy(_fsh);
        if (bgfx::isValid(_vsh)) bgfx::destroy(_vsh);
        if (bgfx::isValid(_ph))  bgfx::destroy(_ph);   
        
        return 0;
    }
}
