#ifndef ATPT_BUFFER_HPP
#define ATPT_BUFFER_HPP

#include <array>

namespace atpt{
 
    template <typename T, size_t N>
    class Buffer{
         
        //+   Member Variable    +//
                std::array<T, N> _contents;
        mutable size_t           _current;
        

        //+   Static Function    +//
        template <typename... As, size_t... Ns>
        static auto _make_array(std::index_sequence<Ns...>, As&&... as_) -> std::array<T, N>
        {
            return { (static_cast<void>(Ns), T(std::forward<As>(as_)...))... };
        }


        //+   Member Function    +//
        private:
        public:
        //_ Constructor
        template <typename... As>
        Buffer (As&&... as_) : _contents (_make_array(std::make_index_sequence<N>{}, std::forward<As>(as_)...)), _current {0} { return; }
        
        //_ Constant Getter
        template <size_t M> inline auto get (void) const -> std::enable_if_t<(M < N), const T&>;
        
        //_ Getter
        template <size_t M> inline auto get (void) -> std::enable_if_t<(M < N), T&>;

        //_ Constant Functin
        inline void timestep (void) const;
    };
}

#include "buffer.inl"

#endif
