#ifndef __CHECKERS_HPP__
#define __CHECKERS_HPP__

#include "../resources/headers/Board.hpp"
#include "../resources/headers/Piece.hpp"

#include <iostream>

namespace SamHovhannisyan::Checkers
{
    class Game
    {
    private:
        enum BoardElements : int
        {
            EMPTY,
            WHITE,
            BLACK,
            WHITE_KING,
            BLACK_KING
        };
    
    public:
        typedef Piece::Piece<BoardElements> Piece;
        typedef Coordinate::Coordinate Coordinate;

    public:
        Game();
        void start();
    
    private:
        void generateDefaultBoard();
        void drawBoard() const;
        void handleInput();
        void movePiece(const Coordinate& from, const Coordinate& to);
        void takePiece(const Coordinate& coord);
        void changePlayer();
        void promote(const Coordinate& coord);
        Piece& getPiece(const Coordinate& coord);
        const Piece& getPiece(const Coordinate& coord) const;
        bool isLegalMove(const Coordinate& from, const Coordinate& to) const;
        bool isWin() const;
        bool isDraw() const;

    private:
        bool game_over_;
        bool player_turn_;
        Board::Board<Piece> board_;
        std::pair<int, int> players_pieces_;
    };
}

#endif
