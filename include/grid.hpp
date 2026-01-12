#ifndef ATPT_GRID_HPP
#define ATPT_GRID_HPP

#include <array>
#include <vector>
#include <functional>

namespace atpt{
 
    template <typename T>
    class FreeBoundaryGrid{
         
        //+   Member Variable    +//
              int            _width;
              int            _height;
              std::vector<T> _vector;
        const T              _default_value;
              T              _dummy_value;
        

        //+   Member Function    +//
        public:
        //_ Constructor
        inline FreeBoundaryGrid (int, int, const T&);

        private:
        //_ Constant Function
        inline auto _is_valid_pos (int, int) const -> bool;

        public:
        //_ Constant Getter
               auto operator [] (size_t id_) const -> const T& { return _vector[id_]; }
        inline auto operator () (int, int)   const -> const T&;
        inline auto neumann     (int, int)   const ->       std::array<const T*, 5>;
        inline auto moore       (int, int)   const ->       std::array<const T*, 9>;

               auto width  (void) const ->       int                                     { return _width; }
               auto height (void) const ->       int                                     { return _height; }
               auto vector (void) const -> const std::vector<T>&                         { return _vector; }
               auto begin  (void) const ->       typename std::vector<T>::const_iterator { return _vector.begin(); };
               auto end    (void) const ->       typename std::vector<T>::const_iterator { return _vector.end(); };

        public:
        //_ Getter
               auto operator [] (size_t id_) -> T& { return _vector[id_]; }
        inline auto operator () (int, int)   -> T&;
               auto begin       (void)       -> typename std::vector<T>::iterator { return _vector.begin(); };
               auto end         (void)       -> typename std::vector<T>::iterator { return _vector.end(); };

        public:
        //_ Variable Function
        inline auto resize (int, int) -> int;
    };



    template <typename T>
    class PeriodicBoundaryGrid{
         
        //+   Member Variable    +//
              int            _width;
              int            _height;
              std::vector<T> _vector;
        const T              _default_value;

        public:
        //+   Member Function    +//
        //_ Constructor
        inline PeriodicBoundaryGrid (int, int, const T&);

        private:
        //_ Constant Function
        inline auto _xmod (int) const -> int;
        inline auto _ymod (int) const -> int;

        public:
        //_ Constant Getter
               auto operator [] (size_t id_) const -> const T& { return _vector[id_]; }
        inline auto operator () (int x_, int y_) const -> const T&;
        inline auto neumann     (int x_, int y_) const ->       std::array<const T*, 5>;
        inline auto moore       (int x_, int y_) const ->       std::array<const T*, 9>;

               auto width  (void) const -> int { return _width; }
               auto height (void) const -> int { return _height; }
               auto vector (void) const -> const std::vector<T>& { return _vector; }
               auto beginv (void) const ->       typename std::vector<T>::const_iterator { return _vector.begin(); };
               auto end    (void) const ->       typename std::vector<T>::const_iterator { return _vector.end(); };

        public:
        //_ Getter
               auto operator [] (size_t id_) -> T& { return _vector[id_]; }
        inline auto operator () (int, int) -> T&;
               auto begin       (void)     -> typename std::vector<T>::iterator { return _vector.begin(); };
               auto end         (void)     -> typename std::vector<T>::iterator { return _vector.end(); };
  
        public:
        //_ Variable Function
        inline auto resize (int, int) -> int;
    };

}

#include "grid.inl"

#endif
