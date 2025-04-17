#ifndef __FRUIT_HPP__
#define __FRUIT_HPP__

#include "../resources/headers/Coordinate.hpp"
#include <iostream>

namespace SamHovhannisyan
{
namespace Fruit
{
    struct Fruit
    {
        typedef Coordinate::Coordinate::coordinate_type coordinate_type;
        static size_t FRUIT_COUNT;
        Coordinate::Coordinate coordinate;
        Fruit(const Coordinate::Coordinate& coord = Coordinate::Coordinate()) : coordinate(coord) {}
        Fruit(const coordinate_type x, const coordinate_type y) : coordinate(Coordinate::Coordinate(x, y)) {}
        Fruit(const Fruit& rhv) : coordinate(rhv.coordinate) {}
        const Fruit& operator=(const Fruit& rhv) { coordinate = rhv.coordinate; return *this; }
        bool operator==(const Fruit& rhv) const { return coordinate == rhv.coordinate; }
        bool operator!=(const Fruit& rhv) const { return !(*this == rhv); }
    };

    inline size_t Fruit::FRUIT_COUNT = 0;
}    
}

#endif // __FRUIT_HPP__
