#ifndef ATPT_GRID_INL
#define ATPT_GRID_INL

#include "grid.hpp"

namespace atpt{

    template <typename T>
    FreeBoundaryGrid<T>::FreeBoundaryGrid (int width_, int height_, const T& dv_)
        : _width         (width_)
        , _height        (height_)
        , _vector        (_width * _height, dv_)
        , _default_value (dv_)
        , _dummy_value   (_default_value)
    {
        return;
    }


    template <typename T>
    auto FreeBoundaryGrid<T>::_is_valid_pos (int x_, int y_) const
        -> bool
    {
        if (x_ < 0 or x_ > _width - 1 or y_ < 0 or y_ > _height - 1) return false;
        else                                                         return true;
    }


    template <typename T>
    auto FreeBoundaryGrid<T>::operator () (int x_, int y_) const
        -> const T&
    {
        return _vector[y_ * _width + x_];
    }


    template <typename T>
    auto FreeBoundaryGrid<T>::neumann (int x_, int y_) const
        -> std::array<const T*, 5>
    {
        std::array<const T*, 5> out{};

        const int center = y_ * _width + x_;
        
        if (_is_valid_pos(x_, y_ - 1)) out[0] = &_vector[center - _width];
        else                           out[0] = &_default_value;
  
        if (_is_valid_pos(x_ - 1, y_)) out[1] = &_vector[center - 1];
        else                           out[1] = &_default_value;
 
        if (_is_valid_pos(x_, y_))     out[2] = &_vector[center];
        else                           out[2] = &_default_value;
 
        if (_is_valid_pos(x_ + 1, y_)) out[3] = &_vector[center + 1];
        else                           out[3] = &_default_value;
     
        if (_is_valid_pos(x_, y_ + 1)) out[4] = &_vector[center + _width];
        else                           out[4] = &_default_value;
    
        return out;
    }


    template <typename T>
    auto FreeBoundaryGrid<T>::moore (int x_, int y_) const
        -> std::array<const T*, 9>
    {
        std::array<const T*, 9> out{};

        const size_t center = y_ * _width + x_;
 
        if (_is_valid_pos(x_ - 1, y_ - 1)) out[0] = &_vector[center - _width - 1];
        else                               out[0] = &_default_value;
       
        if (_is_valid_pos(x_, y_ - 1))     out[1] = &_vector[center - _width];
        else                               out[1] = &_default_value;
 
        if (_is_valid_pos(x_ + 1, y_ - 1)) out[2] = &_vector[center - _width + 1];
        else                               out[2] = &_default_value;
  
        if (_is_valid_pos(x_ - 1, y_))     out[3] = &_vector[center - 1];
        else                               out[3] = &_default_value;
 
        if (_is_valid_pos(x_, y_))         out[4] = &_vector[center];
        else                               out[4] = &_default_value;
 
        if (_is_valid_pos(x_ + 1, y_))     out[5] = &_vector[center + 1];
        else                               out[5] = &_default_value;
 
        if (_is_valid_pos(x_ - 1, y_ + 1)) out[6] = &_vector[center + _width - 1];
        else                               out[6] = &_default_value;
     
        if (_is_valid_pos(x_, y_ + 1))     out[7] = &_vector[center + _width];
        else                               out[7] = &_default_value;
 
        if (_is_valid_pos(x_ + 1, y_ + 1)) out[8] = &_vector[center + _width + 1];
        else                               out[8] = &_default_value;
   
        return out;
    }


    template <typename T>
    auto FreeBoundaryGrid<T>::operator () (int x_, int y_) 
        ->T&
    {
        return _vector[y_ * _width + x_];
    }


    template <typename T>
    auto FreeBoundaryGrid<T>::resize (int nw_, int nh_)
        -> int
    {
        std::vector<T> nvec(nw_ * nh_, _default_value);
        for(size_t a = 0; a < std::min(_height, nh_); ++a){
            std::copy(
                _vector.begin() + a * _width, 
                _vector.begin() + a * _width + std::min(_width, nw_),
                nvec.begin() + a * nw_
            );
        }

        _vector = std::move(nvec);
        _width  = nw_;
        _height = nh_;
        
        return 0;
    }




    template <typename T>
    PeriodicBoundaryGrid<T>::PeriodicBoundaryGrid (int width_, int height_, const T& dv_)
        : _width         (width_)
        , _height        (height_)
        , _vector        (_width * _height, dv_)
        , _default_value (dv_)
    {
        return;
    }


    template <typename T>
    auto PeriodicBoundaryGrid<T>::_xmod (int x_) const
        -> int
    {
        return ((x_ % _width) + _width) % _width;
    }


    template <typename T>
    auto PeriodicBoundaryGrid<T>::_ymod (int y_) const
        -> int
    {
        return ((y_ % _height) + _height) % _height;
    }



    template <typename T>
    auto PeriodicBoundaryGrid<T>::operator () (int x_, int y_) const
        -> const T&
    {
        return _vector[y_ * _width + x_];
    }


    template <typename T>
    auto PeriodicBoundaryGrid<T>::neumann (int x_, int y_) const
        -> std::array<const T*, 5>
    {
        std::array<const T*, 5> out{};
    
        int xm1 = _xmod(x_ - 1);
        int xp1 = _xmod(x_ + 1);
        int ym1 = _ymod(y_ - 1);
        int yp1 = _ymod(y_ + 1);

        out[0] = &(this->operator()(x_,  ym1));
        out[1] = &(this->operator()(xm1, y_));
        out[2] = &(this->operator()(x_,  y_));
        out[3] = &(this->operator()(xp1, y_));
        out[4] = &(this->operator()(x_,  yp1));
        
        return out;
    }


    template <typename T>
    auto PeriodicBoundaryGrid<T>::moore (int x_, int y_) const
        -> std::array<const T*, 9>
    {
        std::array<const T*, 9> out{};
    
        int xm1 = _xmod(x_ - 1);
        int xp1 = _xmod(x_ + 1);
        int ym1 = _ymod(y_ - 1);
        int yp1 = _ymod(y_ + 1);

        out[0] = &(this->operator()(xm1, ym1));
        out[1] = &(this->operator()(x_,  ym1));
        out[2] = &(this->operator()(xp1, ym1));
        out[3] = &(this->operator()(xm1, y_));
        out[4] = &(this->operator()(x_,  y_));
        out[5] = &(this->operator()(xp1, y_));
        out[6] = &(this->operator()(xm1, yp1));
        out[7] = &(this->operator()(x_,  yp1));
        out[8] = &(this->operator()(xp1, yp1));
        
        return out;
    }


    template <typename T>
    auto PeriodicBoundaryGrid<T>::operator () (int x_, int y_) 
        ->T&
    {
        return _vector[y_ * _width + x_];
    }


    template <typename T>
    auto PeriodicBoundaryGrid<T>::resize (int nw_, int nh_)
        -> int
    {
        std::vector<T> nvec(nw_ * nh_, _default_value);
        for(size_t a = 0; a < std::min(_height, nh_); ++a){
            std::copy(
                _vector.begin() + a * _width, 
                _vector.begin() + a * _width + std::min(_width, nw_),
                nvec.begin() + a * nw_
            );
        }

        _vector = std::move(nvec);
        _width  = nw_;
        _height = nh_;
        return 0;
    }
}
#endif
