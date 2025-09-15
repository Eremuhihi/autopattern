#include <noise.hpp>
#include <cstdint>

namespace atpt{

    Noise::Noise (SDL_Window* wd_, uint32_t seed_)
        : Panel    ( "Noise", wd_, "shaders/fs_texture.bin" )
        , _uh      ( bgfx::createUniform("u_tex0", bgfx::UniformType::Sampler) )
        , _th      ( bgfx::createTexture2D(static_cast<uint16_t>(_width), static_cast<uint16_t>(_height), false, 1, bgfx::TextureFormat::BGRA8, 0) )
        , _pixels ( _width * _height )
        , _seed    { seed_ }
        , _mt      ( _seed )
    {
        return;
    }

    
    auto Noise::_resize (int o_width_, int o_height_)
        -> int
    {
        _pixels.assign( static_cast<size_t>(this->_width) * static_cast<size_t>(this->_height), 0);
       
        if (bgfx::isValid(_th)) bgfx::destroy(_th);
        _th = bgfx::createTexture2D(static_cast<uint16_t>(_width), static_cast<uint16_t>(_height), false, 1, bgfx::TextureFormat::BGRA8, 0); 
        
        return 0;
    }


    auto Noise::_draw (void)
        -> int
    {
        for (size_t i = 0; i < _pixels.size(); ++i) {
            _pixels[i] = (_mt() & 1) ? 0xFFFFFFFFu : 0xFF000000u;
        }

        // pixels → GPUに転送
        const bgfx::Memory* mem = bgfx::copy(
            _pixels.data(), (uint32_t)(_pixels.size() * sizeof(uint32_t))
        );
        bgfx::updateTexture2D(_th, 0, 0, 0, 0, static_cast<uint16_t>(_width), static_cast<uint16_t>(_height), mem);

        bgfx::setTexture(0, _uh, _th);
        
        return 0;
    }


    auto Noise::_event (const SDL_Event&)
        -> int
    {
        return 0;
    }


    auto Noise::_destroy (void)
        -> int
    {
        if (bgfx::isValid(_th)) bgfx::destroy(_th);
        if (bgfx::isValid(_uh)) bgfx::destroy(_uh);
        
        return 0;
    }
}