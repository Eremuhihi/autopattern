#include <panel_set.hpp>

namespace atpt{

    //+    Member Function    +//
    //_ Constructor
    PanelSet::PanelSet (SDL_Window* wh_)
        : _panels   ( )
        , _panel_id (0)
        , _wh       (wh_)
    {
        return;
    }


    //_ Variable Function
    auto PanelSet::draw (void) -> int
    {
        return _panels.at(_panel_id)->draw();        
    }


    auto PanelSet::event (const SDL_Event& e_) -> int
    {
        if (e_.type == SDL_KEYDOWN) {
            switch (e_.key.keysym.sym) {
                case SDLK_UP:
                    if (_panel_id == _panels.size() - 1)   _panel_id = 0;
                    else                                 ++_panel_id;
                    SDL_SetWindowTitle(_wh, name().c_str());
                    return 1;

                case SDLK_DOWN:
                    if (_panel_id == 0)   _panel_id = _panels.size() - 1;
                    else                --_panel_id;
                    SDL_SetWindowTitle(_wh, name().c_str());
                    return 1;
            }
        }

        return _panels.at(_panel_id)->event(e_);
    }


    auto PanelSet::destroy (void) -> int
    {
        return _panels.at(_panel_id)->destroy();
    }
}
