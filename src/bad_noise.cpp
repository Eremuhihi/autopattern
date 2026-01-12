#include <bad_noise.hpp>
#include <cstdint>

namespace atpt{

    BadNoise::BadNoise (SDL_Window* wd_, uint32_t seed_)
        : Panel    ( "Bad Noise", wd_, "shaders/fs_texture.bin" )
        , _uh      ( bgfx::createUniform("u_tex0", bgfx::UniformType::Sampler) )
        , _th      ( bgfx::createTexture2D(static_cast<uint16_t>(_width), static_cast<uint16_t>(_height), false, 1, bgfx::TextureFormat::BGRA8, 0) )
        , _pixels  ( _width * _height )
        , _seed    { seed_ }
        , _invar   ( _seed )
    {
        return;
    }

    
    auto BadNoise::_resize (int o_width_, int o_height_)
        -> int
    {
        _pixels.assign( static_cast<size_t>(this->_width) * static_cast<size_t>(this->_height), 0);
       
        if (bgfx::isValid(_th)) bgfx::destroy(_th);
        _th = bgfx::createTexture2D(static_cast<uint16_t>(_width), static_cast<uint16_t>(_height), false, 1, bgfx::TextureFormat::BGRA8, 0); 
        
        return 0;
    }


    auto BadNoise::_draw (void)
        -> int
    {
        for (size_t i = 0; i < _pixels.size(); ++i) {
            uint32_t tmp = ((_invar >> 0) ^ (_invar >> 2)) & 1;
            _invar = (_invar >> 1) | (tmp << 31);
            _pixels[i] = (_invar & 1) ? 0xFF13A00Eu : 0xFF000000u;
        }

        // pixels → GPUに転送
        const bgfx::Memory* mem = bgfx::copy(
            _pixels.data(), (uint32_t)(_pixels.size() * sizeof(uint32_t))
        );
        bgfx::updateTexture2D(_th, 0, 0, 0, 0, static_cast<uint16_t>(_width), static_cast<uint16_t>(_height), mem);

        bgfx::setTexture(0, _uh, _th);
        
        return 0;
    }


    auto BadNoise::_event (const SDL_Event& e_)
        -> int
    {
        if (e_.type == SDL_KEYDOWN){
            switch (e_.key.keysym.sym) {
                case SDLK_UP:
                    ++_invar;
                    break;
                case SDLK_DOWN:
                    --_invar;
                    break;
                default:
                    break;
            }
        }
        return 0;
    }


    auto BadNoise::_destroy (void)
        -> int
    {
        if (bgfx::isValid(_th)) bgfx::destroy(_th);
        if (bgfx::isValid(_uh)) bgfx::destroy(_uh);
        
        return 0;
    }
}
