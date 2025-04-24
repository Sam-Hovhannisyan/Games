#ifndef __COORDINATE_HPP__
#define __COORDINATE_HPP__

#include <iostream>

/// @brief Namespace for the Coordinate class
/// @details This namespace contains the Coordinate class which represents a 2D coordinate.
/// @namespace Coordinate
namespace SamHovhannisyan::Coordinate
{   
    /// @brief Class representing a 2D coordinate
    /// @details This class contains two coordinate types: x and y. It also provides constructors,
    ///          assignment operator, equality operator, and inequality operator.
    /// @class Coordinate
    /// @tparam coordinate_type The type of the coordinates (default is size_t)
    /// @param x The x coordinate (default is 0)
    /// @param y The y coordinate (default is 0)
    /// @param rhv The right-hand value for copy constructor and assignment operator
    struct Coordinate
    {
        typedef size_t coordinate_type;
        coordinate_type x;
        coordinate_type y;
        /// @brief Default constructor
        /// @details Initializes the x and y coordinates to 0.
        Coordinate(const coordinate_type x = 0, const coordinate_type y = 0) : x(x), y(y) {}
        /// @brief Copy constructor
        /// @details Initializes the x and y coordinates to the values of the right-hand value.
        /// @param rhv The right-hand value for copy constructor
        Coordinate(const Coordinate& rhv) : x(rhv.x), y(rhv.y) {}
        /// @brief Assignment operator
        /// @details Assigns the x and y coordinates of the right-hand value to this object.
        /// @param rhv The right-hand value for assignment operator
        const Coordinate& operator=(const Coordinate& rhv) { x = rhv.x; y = rhv.y; return *this; }
        /// @brief Equality operator
        /// @details Compares the x and y coordinates of this object with the right-hand value.
        /// @param rhv The right-hand value for equality operator
        bool operator==(const Coordinate& rhv) const { return x == rhv.x && y == rhv.y; }
        /// @brief Inequality operator
        /// @details Compares the x and y coordinates of this object with the right-hand value.
        /// @param rhv The right-hand value for inequality operator
        bool operator!=(const Coordinate& rhv) const { return !(*this == rhv); }
    };
}

#endif
