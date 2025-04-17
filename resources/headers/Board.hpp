// Header guard to prevent multiple inclusions
#ifndef __BOARD_HPP__ 
#define __BOARD_HPP__ 

#include "../headers/Coordinate.hpp"

#include <iostream>
#include <vector>
#include <cassert>

/// @brief Namespace for the SamHovhannisyan project
/// @details This namespace contains all the classes and functions related to the project.
/// @namespace SamHovhannisyan
namespace SamHovhannisyan
{
/// @brief Namespace for the Board class
/// @details This namespace contains the Board class which represents a 2D board.
/// @namespace Board
namespace Board
{
    /// @brief Class representing a 2D board
    /// @details This class contains a 2D vector to represent the board, and provides methods to
    ///          get and set the size of the board, get the board, and access elements using
    ///          coordinates. It also provides a constructor, copy constructor, and assignment operator.
    /// @class Board
    /// @tparam T The type of the elements in the board
    /// @param rhv The right-hand value for copy constructor and assignment operator
    /// @param size_type The size type for the board
    /// @param board_ The 2D vector representing the board
    /// @param rows_ The number of rows in the board
    /// @param cols_ The number of columns in the board
    /// @param coordinate The Coordinate class representing the coordinates of the board
    /// @param coordinate_type The type of the coordinates (default is size_t)
    template <typename T>
    class Board
    {
    public:
        typedef Coordinate::Coordinate coordinate;
        typedef typename coordinate::coordinate_type coordinate_type;
        typedef size_t size_type;

    public:
        /// @brief Constructor
        /// @details Initializes the board with the given number of rows and columns.
        /// @param rows The number of rows in the board (default is 0)
        /// @param cols The number of columns in the board (default is 0)
        Board(const size_type rows = 0, const size_type cols = 0);
        /// @brief Copy constructor
        /// @details Initializes the board with the values of the right-hand value.
        /// @param rhv The right-hand value for copy constructor
        Board(const Board& rhv);
        /// @brief Assignment operator
        /// @details Assigns the values of the right-hand value to this object.
        /// @param rhv The right-hand value for assignment operator
        /// @return A reference to this object
        /// @details This operator allows for the assignment of one Board object to another.
        const Board& operator=(const Board& rhv);
        
        std::vector<std::vector<T>> getBoard() const;
        size_type getRows() const;
        size_type getCols() const;
        
        void setRows(const size_type rows);
        void setCols(const size_type cols);
        
        /// @brief Access operators
        /// @details This operators allow for the access of elements in the board using coordinates.
        /// @param coord The coordinates of the element to access
        /// @return A reference to the element at the given coordinates
        T& operator()(const coordinate& coord);
        const T& operator()(const coordinate& coord) const;

        /// @brief Set value at the given coordinates
        /// @details This method allows for the setting of a value at the given coordinates.
        /// @param coord The coordinates of the element to set
        /// @param value The value to set at the given coordinates
        void setValue(const coordinate& coord, const T& value);
        
    private:
        size_type rows_;
        size_type cols_;
        std::vector<std::vector<T>> board_;
    };
}
}

#include "../templates/Board.cpp"

#endif // __BOARD_HPP__
