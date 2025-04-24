#ifndef __PIECE_HPP__
#define __PIECE_HPP__

namespace SamHovhannisyan::Piece
{
    template <typename T>
    struct Piece
    {
        T value;
        bool hasMoved;

        Piece(const T& val = T(), bool hasMoved = false) : value(val), hasMoved(hasMoved) {}
        Piece(const Piece& rhv) : value(rhv.value), hasMoved(rhv.hasMoved) {}
        const Piece& operator=(const Piece& rhv) { value = rhv.value; hasMoved = rhv.hasMoved; return *this; }
        bool operator==(const Piece& rhv) const { return value == rhv.value; }
        bool operator!=(const Piece& rhv) const { return !(*this == rhv); }
    };
}

#endif
