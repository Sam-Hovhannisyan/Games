#ifndef __GAME_HPP__
#define __GAME_HPP__

#include "../resources/headers/Board.hpp"

#include <algorithm>
#include <random>

namespace SamHovhannisyan
{
namespace MinesweeperGame
{
    class Game
    {
    public:
        Game(const size_t width = 16, const size_t height = 16);
        void start();
    
    private:
        enum BoardElements : int
        {
            EMPTY,
            ONE,
            TWO,
            THREE,
            FOUR,
            FIVE,
            SIX,
            SEVEN,
            EIGHT,
            MINE,
            FLAG
        };
    
    private:
        void drawBoard() const;
        void generateMines();
        void placeBomb(const size_t pos);
        void handleInput();
        void checkCollision();
        bool checkWin() const;
        void openEmptysFrom(const Coordinate::Coordinate& coord);
        void openCell(const Coordinate::Coordinate& coord);
        // void ensureSafeFirstClick(const Coordinate::Coordinate& coord);
        // void updateAdjacentNumbers(const Coordinate::Coordinate& coord);
        void revealAllMines();

    private:
        Board::Board<std::pair<BoardElements, bool>> board_;
        // bool first_click_;
        bool game_over_;
        size_t mines_count_;
        size_t flags_placed_;
    };
}    
}

#endif // __GAME_HPP__
