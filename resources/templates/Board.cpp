#include "headers/Board.hpp"

namespace SamHovhannisyan
{
namespace Board
{
    template <typename T>
    Board<T>::Board(const size_type rows, const size_type cols)
        : rows_(rows)
        , cols_(cols)
        , board_(rows, std::vector<T>(cols, T()))
    {}

    template <typename T>
    Board<T>::Board(const Board& rhv)
        : rows_(rhv.rows_)
        , cols_(rhv.cols_)
        , board_(rhv.board_) 
    {}

    template <typename T>
    const Board<T>&
    Board<T>::operator=(const Board& rhv)
    {
        if (this != &rhv) {
            rows_ = rhv.rows_;
            cols_ = rhv.cols_;
            board_ = rhv.board_;
        }
        return *this;
    }

    template <typename T>
    std::vector<std::vector<T>> 
    Board<T>::getBoard() const
    {
        return board_;
    }

    template <typename T>
    typename Board<T>::size_type 
    Board<T>::getRows() const
    {
        return rows_;
    }

    template <typename T>
    typename Board<T>::size_type 
    Board<T>::getCols() const
    {
        return cols_;
    }

    template <typename T>
    void 
    Board<T>::setRows(const size_type rows)
    {
        rows_ = rows;
    }

    template <typename T>
    void
    Board<T>::setCols(const size_type cols)
    {
        cols_ = cols;
    }

    template <typename T>
    T&
    Board<T>::operator()(const coordinate& coord)
    {
        assert(coord.x < getRows());
        assert(coord.y < getCols());
        return board_[coord.x][coord.y];
    }

    template <typename T>
    const T&
    Board<T>::operator()(const coordinate& coord) const
    {
        assert(coord.x < getRows());
        assert(coord.y < getCols());
        return board_[coord.x][coord.y];
    }

    template <typename T>
    void 
    Board<T>::setValue(const coordinate& coord, const T& value)
    {
        assert(coord.x < getRows());
        assert(coord.y < getCols());
        board_[coord.x][coord.y] = value;
    }
}
}
