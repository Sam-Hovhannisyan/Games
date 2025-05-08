#ifndef __SNAKE_HPP__
#define __SNAKE_HPP__

#include "../resources/headers/Board.hpp"
#include "../headers/Fruit.hpp"

#include <ncurses.h>
#include <unistd.h>
#include <ctime>
#include <cstdlib>

namespace SamHovhannisyan::SnakeGame
{
    class Game
    {
    public:
        Game(const size_t width = 20, const size_t height = 20);
        void start();
    
    private:
        enum Direction
        {
            UP = 8,
            DOWN = 2,
            LEFT = 4,
            RIGHT = 6
        };

        enum BoardElements
        {
            EMPTY = 0,
            SNAKE_HEAD = 1,
            SNAKE_BODY = 2,
            FRUIT = 3
        };

    private:
        void drawBoard() const;
        void moveSnake();
        void placeFruit();
        void eatFruit();
        void changeDirection(Direction newDirection);
        void checkCollision();
        void initializeColors();
        void handleInput();
        void renderGameOver() const;
    
    private:
        Board::Board<BoardElements> board_;
        Fruit::Fruit fruit_;
        Coordinate::Coordinate snakeHead_;
        std::vector<Coordinate::Coordinate> snakeBody_;
        Direction direction_;
        size_t level_;
        size_t speed_;
        bool game_over_;
    };
}    

#endif // __SNAKE_HPP__
