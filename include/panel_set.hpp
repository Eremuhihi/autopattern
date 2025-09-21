#ifndef ATPT_PANEL_SET_HPP
#define ATPT_PANEL_SET_HPP

#include <iostream>
#include <panel.hpp>

namespace atpt{

    class PanelSet{

        using PanelPtr = std::unique_ptr<Panel>;
        
        protected:
        //+    Member Variable    +//
                std::vector<PanelPtr> _panels;
                size_t                _panel_id;
        mutable SDL_Window*           _wh;

        public:
        //+    Member Function    +//
        //_ Consructor
        PanelSet (SDL_Window*);

        //_ Getter
        auto name     (void)       const -> const std::string& { return _panels.at(_panel_id)->name(); }
        auto panel_id (void)       const -> size_t             { return _panel_id; }
        auto panel    (size_t id_) const -> const Panel&       { return *_panels.at(id_); }
        auto panel    (void)       const -> const Panel&       { return *_panels.at(_panel_id); }
        auto window   (void)       const -> const SDL_Window*  { return _wh; }
        
        //_ Setter
        auto panel     (size_t id_) -> Panel& { return *_panels.at(id_); }
        auto panel     (void)       -> Panel& { return *_panels.at(_panel_id); }
        
        template <class A, typename... As> inline auto createPanel (As&&...) -> A&;
        
        //_ Variable Function
        auto draw    (void)             -> int;
        auto event   (const SDL_Event&) -> int;
        auto destroy (void)             -> int;
    };

        
    
    template <class A, typename... As>
    auto PanelSet::createPanel (As&&... as_) -> A&
    {
        _panels.push_back(std::make_unique<A>(std::forward<As>(as_)...));
        
        if (_wh != _panels.back().get()->window()){
            std::cerr << "window handle is different" << std::endl;
            std::exit(1);
        }

        return *static_cast<A*>(_panels.back().get());
    }
}

#endif
