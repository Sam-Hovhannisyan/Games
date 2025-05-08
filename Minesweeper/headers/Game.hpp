#ifndef __MINESWEEPER_HPP__
#define __MINESWEEPER_HPP__

#include "../resources/headers/Board.hpp"

#include <algorithm>
#include <random>

namespace SamHovhannisyan::MinesweeperGame
{
    class Game
    {
    public:
        typedef Coordinate::Coordinate Coordinate;
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
        void generateMines(const Coordinate& coord);
        void placeBomb(const size_t pos);
        void placeRemoveFlag(const Coordinate& coord);
        std::vector<SamHovhannisyan::MinesweeperGame::Game::Coordinate>::const_iterator getFlag(const Coordinate& coord) const;
        const Coordinate handleInput();
        void checkCollision();
        bool checkWin() const;
        void openEmptysFrom(const Coordinate& coord);
        void openCell(const Coordinate& coord);
        void revealAllMines();

    private:
        Board::Board<std::pair<BoardElements, bool>> board_;
        std::vector<Coordinate> flags_;
        bool first_click_;
        bool game_over_;
        size_t mines_count_;
        size_t flags_placed_;
    };
}    

#endif
