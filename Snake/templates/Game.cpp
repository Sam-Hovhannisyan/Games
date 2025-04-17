#include "headers/Game.hpp"
#include <algorithm>

namespace SamHovhannisyan 
{
namespace SnakeGame 
{
    Game::Game(const size_t width, const size_t height) 
        : board_(width, height)
        , direction_(RIGHT)
        , level_(1)
        , speed_(150000)
        , game_over_(false)
    {
        std::srand(std::time(0));
        snakeHead_.x = width / 2;
        snakeHead_.y = height / 2;
        snakeBody_.push_back(snakeHead_);
        placeFruit();
    }

    void 
    Game::start() 
    {
        std::srand(time(nullptr));
        initscr();
        noecho();
        cbreak();
        keypad(stdscr, TRUE);
        nodelay(stdscr, TRUE);
        initializeColors();

        while (!game_over_) 
        {
            handleInput();
            moveSnake();
            checkCollision();
            drawBoard();
            usleep(speed_);
        }

        renderGameOver();
        endwin();
    }

    void 
    Game::drawBoard() const
    {
        clear();
        
        // Draw game area with border offset
        for (size_t y = 0; y < board_.getRows(); ++y) {
            for (size_t x = 0; x < board_.getCols(); ++x) {
                Coordinate::Coordinate pos(x, y);
                int displayChar = '.';
                int colorPair = 0;
                
                if (pos == snakeHead_) {
                    displayChar = '@';
                    colorPair = 1;
                }
                else if (std::find(snakeBody_.begin(), snakeBody_.end(), pos) != snakeBody_.end()) {
                    displayChar = 'O';
                    colorPair = 1;
                }
                else if (pos == fruit_.coordinate) {
                    displayChar = '*';
                    colorPair = 2;
                }
                
                mvaddch(y + 1, x + 1, displayChar | COLOR_PAIR(colorPair));
            }
        }
        
        // Draw border
        attron(COLOR_PAIR(3));
        for (size_t x = 0; x < board_.getCols() + 2; x++) {
            mvaddch(0, x, '#');
            mvaddch(board_.getRows() + 1, x, '#');
        }
        for (size_t y = 0; y < board_.getRows() + 2; y++) {
            mvaddch(y, 0, '#');
            mvaddch(y, board_.getCols() + 1, '#');
        }
        attroff(COLOR_PAIR(3));
        
        // Draw score
        mvprintw(0, 2, "Score: %zu", snakeBody_.size());
        
        refresh();
    }

    void 
    Game::moveSnake() 
    {
        Coordinate::Coordinate newHead = snakeHead_;
        switch (direction_) 
        {
            case UP:    --newHead.y; break;
            case DOWN:  ++newHead.y; break;
            case LEFT:  --newHead.x; break;
            case RIGHT: ++newHead.x; break;
        }

        snakeBody_.insert(snakeBody_.begin(), newHead);
        if (newHead == fruit_.coordinate) 
        {
            eatFruit();
        } 
        else 
        {
            snakeBody_.pop_back();
        }
        snakeHead_ = newHead;
    }

    void
    Game::changeDirection(Direction newDirection) 
    {
        if ((direction_ == UP    && newDirection != DOWN ) ||
            (direction_ == DOWN  && newDirection != UP   ) ||
            (direction_ == LEFT  && newDirection != RIGHT) ||
            (direction_ == RIGHT && newDirection != LEFT)) 
        {
            direction_ = newDirection;
        }
    }

    void
    Game::checkCollision()
    {
        if (snakeHead_.x >= board_.getCols() ||
            snakeHead_.y >= board_.getRows()) 
        {
            game_over_ = true;
            return;
        }

        for (size_t i = 1; i < snakeBody_.size(); ++i) 
        {
            if (snakeHead_ == snakeBody_[i]) 
            {
                game_over_ = true;
                return;
            }
        }
    }

    void 
    Game::initializeColors()
    {
        start_color();
        init_pair(1, COLOR_GREEN, COLOR_BLACK);  // Snake
        init_pair(2, COLOR_RED, COLOR_BLACK);    // Fruit
        init_pair(3, COLOR_WHITE, COLOR_BLACK);  // Border
    }

    void 
    Game::handleInput() 
    {
        int ch = getch();
        switch (ch) 
        {
            case KEY_UP:    changeDirection(UP);    break;
            case KEY_DOWN:  changeDirection(DOWN);  break;
            case KEY_LEFT:  changeDirection(LEFT);  break;
            case KEY_RIGHT: changeDirection(RIGHT); break;
        }
    }

    void 
    Game::placeFruit()
    {
        std::vector<Coordinate::Coordinate> emptySpots;
    
        // Find all empty spots on the board
        for (size_t y = 0; y < board_.getRows(); ++y) {
            for (size_t x = 0; x < board_.getCols(); ++x) {
                Coordinate::Coordinate pos(x, y);
                
                // Check if position is not occupied by snake
                if (pos != snakeHead_ && 
                    std::find(snakeBody_.begin(), snakeBody_.end(), pos) == snakeBody_.end()) {
                    emptySpots.push_back(pos);
                }
            }
        }
        
        // If no empty spots left, end game
        if (emptySpots.empty()) {
            game_over_ = true;
            return;
        }
        
        // Select random empty spot
        size_t randomIndex = std::rand() % emptySpots.size();
        fruit_.coordinate = emptySpots[randomIndex];
        Fruit::Fruit::FRUIT_COUNT++;
    }

    void
    Game::eatFruit()
    {
        placeFruit();
        
        if (Fruit::Fruit::FRUIT_COUNT % 3 == 0 && speed_ > 20000) 
        {
            speed_ -= 10000;
            level_++;
        }
    }

    void
    Game::renderGameOver() const
    {
        clear();
        mvprintw(board_.getRows() / 2 - 1, board_.getCols() / 2 - 10, "GAME OVER");
        mvprintw(board_.getRows() / 2 + 1, board_.getCols() / 2 - 15, "Score: %zu", snakeBody_.size());
        mvprintw(board_.getRows() / 2 + 3, board_.getCols() / 2 - 15, "Fruits: %zu", Fruit::Fruit::FRUIT_COUNT);
        mvprintw(board_.getRows() / 2 + 5, board_.getCols() / 2 - 12, "Press any key");
        refresh();
        getch();
    }
}
}