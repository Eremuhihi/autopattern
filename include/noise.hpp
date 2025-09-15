#ifndef ATPT_NOISE_HPP
#define ATPT_NOISE_HPP

#include <panel.hpp>
#include <random>
#include <bgfx/bgfx.h>

namespace atpt{
 
    class Noise : public Panel{
        
        //+   Member Variable    +//
        bgfx::UniformHandle   _uh;
        bgfx::TextureHandle   _th;
        std::vector<uint32_t> _pixels;
        uint32_t              _seed;
        std::mt19937          _mt;

        public:
        //+   Member Function    +//
        //_ Constructor
        Noise (SDL_Window*, uint32_t);

        //_ Getter
        auto seed (void) -> uint32_t { return _seed; }
        
        //_ Variable Function
        auto _resize  (int, int)         -> int override;
        auto _draw    (void)             -> int override;
        auto _event   (const SDL_Event&) -> int override;
        auto _destroy (void)             -> int override;

    };
}

#endif