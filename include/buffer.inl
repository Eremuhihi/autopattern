#ifndef ATPT_BUFFER_INL
#define ATPT_BUFFER_INL

#include "buffer.hpp"

namespace atpt{

    template <typename T, size_t N> template <size_t M>
    auto Buffer<T, N>::get (void) const
        -> std::enable_if_t<(M < N), const T&>
    {
        return (_current + M < N ? _contents[_current + M] : _contents[_current + M - N]);
    }


    template <typename T, size_t N> template <size_t M>
    auto Buffer<T, N>::get (void)
        -> std::enable_if_t<(M < N), T&>
    {
        return (_current + M < N ? _contents[_current + M] : _contents[_current + M - N]);
    }


    template <typename T, size_t N>
    void Buffer<T, N>::timestep (void) const
    {
        _current = _current++ < N ? _current : _current - N;
    }
}
#endif
