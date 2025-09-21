#ifndef ATPT_BAD_NOISE_HPP
#define ATPT_BAD_NOISE_HPP

#include <panel.hpp>
#include <random>
#include <bgfx/bgfx.h>

namespace atpt{
 
    class BadNoise : public Panel{
        
        //+   Member Variable    +//
        bgfx::UniformHandle   _uh;
        bgfx::TextureHandle   _th;
        std::vector<uint32_t> _pixels;
        uint32_t              _seed;
        uint32_t              _invar;

        public:
        //+   Member Function    +//
        //_ Constructor
        BadNoise (SDL_Window*, uint32_t = 0xACE1u);

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