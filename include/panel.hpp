#ifndef ATPT_PANEL_HPP
#define ATPT_PANEL_HPP

#include <SDL2/SDL_events.h>
#include <vector>
#include <string>
#include <filesystem>
#include <bgfx/bgfx.h>

namespace atpt{

    class Panel{

        protected:
        //+    Member Variable    +//
        const std::string              _name;
              SDL_Window*              _wh;
              int                      _width;
              int                      _height;
              bgfx::ShaderHandle       _vsh;
              bgfx::ShaderHandle       _fsh;
              bgfx::ProgramHandle      _ph;
              bgfx::VertexBufferHandle _vbh;
              bgfx::IndexBufferHandle  _ibh;
        

        //+    Member Function    +//
        private:
        //_ Inner Function
        auto _getWidthFromWindowHandle  (void) -> int;
        auto _getHeightFromWindowHandle (void) -> int;

        public:
        //_ Consructor
        Panel (const std::string&, SDL_Window*, const std::filesystem::path&);

        public:
        //_ Destructor
        virtual ~Panel () = default;

        public:
        //_ Getter
        auto width  (void) -> int                { return _width; }
        auto height (void) -> int                { return _height; }
        auto name   (void) -> const std::string& { return _name; }
        auto window (void) -> SDL_Window*        { return _wh; }  
        
        //_ Variable Function
        public:
        auto draw    (void)             -> int;
        auto event   (const SDL_Event&) -> int;
        auto destroy (void)             -> int;
        

        protected:
        virtual auto _resize  (int, int)         -> int = 0;
        virtual auto _destroy (void)             -> int = 0;
        virtual auto _event   (const SDL_Event&) -> int = 0;
        virtual auto _draw    (void)             -> int = 0;
    };

}

#endif
