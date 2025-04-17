#ifndef __BOARD_HPP__
#define __BOARD_HPP__

#include <iostream>
#include <vector>
#include <cassert>

namespace SamHovhannisyan
{
namespace Coordinate
{
    struct Coordinate
    {
        typedef size_t coordinate_type;
        coordinate_type x;
        coordinate_type y;
        Coordinate(const coordinate_type x = 0, const coordinate_type y = 0) : x(x), y(y) {}
        Coordinate(const Coordinate& rhv) : x(rhv.x), y(rhv.y) {}
        const Coordinate& operator=(const Coordinate& rhv) { x = rhv.x; y = rhv.y; return *this; }
        bool operator==(const Coordinate& rhv) const { return x == rhv.x && y == rhv.y; }
        bool operator!=(const Coordinate& rhv) const { return !(*this == rhv); }
    };
}

namespace Board
{
    template <typename T>
    class Board
    {
    public:
        typedef Coordinate::Coordinate coordinate;
        typedef typename coordinate::coordinate_type coordinate_type;
        typedef size_t size_type;

    public:
        Board(const size_type rows, const size_type cols);
        Board(const Board& rhv);
        const Board& operator=(const Board& rhv);
        
        const std::vector<std::vector<T>> getBoard() const;
        size_type getRows() const;
        size_type getCols() const;
        
        void setRows(const size_type rows);
        void setCols(const size_type cols);
    
        T& operator()(const coordinate& coord);
        const T& operator()(const coordinate& coord) const;

        void setValue(const coordinate& coord, const T& value);
        
    private:
        std::vector<std::vector<T>> board_;
        size_type rows_;
        size_type cols_;
    };
}
}

#include "templates/board.cpp"

#endif // __BOARD_HPP__
