#include <conway_ca.hpp>
#include <cstdint>

namespace atpt{

    ConwayCA::ConwayCA (SDL_Window* wd_, uint32_t seed_)
        : Panel     ( "ConwayCA", wd_, "shaders/fs_texture.bin" )
        , _uh       ( bgfx::createUniform("u_tex0", bgfx::UniformType::Sampler) )
        , _th       ( bgfx::createTexture2D(static_cast<uint16_t>(_width), static_cast<uint16_t>(_height), false, 1, bgfx::TextureFormat::BGRA8, 0) )
        , _pixels   ( _width * _height )
        , _grid_buf ( _width, _height, 0 )
        , _seed     { seed_ }
        , _mt       ( _seed )
    {
        std::uniform_int_distribution<> d(0, 1);
        for(int& i : _grid_buf.get<1>()) i = d(_mt); 

        return;
    }

    
    auto ConwayCA::_resize (int o_width_, int o_height_)
        -> int
    {
        _grid_buf.get<0>().resize(o_width_, o_height_);
        _grid_buf.get<1>().resize(o_width_, o_height_);

        _pixels.assign( static_cast<size_t>(this->_width) * static_cast<size_t>(this->_height), 0);
       
        if (bgfx::isValid(_th)) bgfx::destroy(_th);
        _th = bgfx::createTexture2D(static_cast<uint16_t>(_width), static_cast<uint16_t>(_height), false, 1, bgfx::TextureFormat::BGRA8, 0); 
        
        return 0;
    }


    auto ConwayCA::_draw (void)
        -> int
    {
        for(size_t b = 0; b < _grid_buf.get<1>().height(); ++b){
            for(size_t a = 0; a < _grid_buf.get<1>().width(); ++a){

                std::array<const int*, 9> moore = _grid_buf.get<1>().moore(a, b);
                int sum = *moore[0] + *moore[1] + *moore[2] +
                          *moore[3]             + *moore[5] +
                          *moore[6] + *moore[7] + *moore[8];

                if (*moore[4] == 0 and sum == 3){
                    _grid_buf.get<0>()(a,b) = 1;
                }else if (*moore[4] == 1 and (sum == 2 or sum == 3)){
                    _grid_buf.get<0>()(a,b) = 1;
                }else{
                    _grid_buf.get<0>()(a,b) = 0;
                }
            }
        }

        for (size_t i = 0; i < _pixels.size(); ++i) {
            _pixels[i] = (_grid_buf.get<0>()[i] & 1) ? 0xFF13A00Eu : 0xFF000000u;
        }

        // pixels → GPUに転送
        const bgfx::Memory* mem = bgfx::copy(
            _pixels.data(), (uint32_t)(_pixels.size() * sizeof(uint32_t))
        );
        bgfx::updateTexture2D(_th, 0, 0, 0, 0, static_cast<uint16_t>(_width), static_cast<uint16_t>(_height), mem);

        bgfx::setTexture(0, _uh, _th);
       
        _grid_buf.timestep();

        return 0;
    }


    auto ConwayCA::_event (const SDL_Event&)
        -> int
    {
        return 0;
    }


    auto ConwayCA::_destroy (void)
        -> int
    {
        if (bgfx::isValid(_th)) bgfx::destroy(_th);
        if (bgfx::isValid(_uh)) bgfx::destroy(_uh);
        
        return 0;
    }
}
