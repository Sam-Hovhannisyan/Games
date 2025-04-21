#include "../headers/Game.hpp"
#include <cstdlib>
#include <iomanip>
#include <numeric>
#include <ncurses.h>
#include <queue>

namespace SamHovhannisyan::MinesweeperGame
{
    size_t mouseX = 0;
    size_t mouseY = 0;
    bool mouseHover = false;

    Game::Game(const size_t width, const size_t height)
        : board_(width, height)
        // , first_click_(true)
        , game_over_(false)
        , mines_count_(0)
        , flags_placed_(0)
    {
        if (width == 0 || height == 0) { throw std::invalid_argument("Board dimensions cannot be zero"); }

        // Initialize ncurses
        initscr();
        cbreak();
        noecho();
        keypad(stdscr, TRUE);
        mousemask(ALL_MOUSE_EVENTS, NULL);
        curs_set(0); // Hide cursor

        for (size_t y = 0; y < board_.getRows(); ++y) 
        {
            for (size_t x = 0; x < board_.getCols(); ++x) 
            {
                board_({y, x}) = {BoardElements::EMPTY, false};
            }
        }
    }

    void 
    Game::drawBoard() const
    {
        clear();
        
        // Draw header
        attron(A_BOLD);
        printw("Minesweeper - Mines: %zu | Flags: %zu\n\n", mines_count_, flags_placed_);
        attroff(A_BOLD);

        // Draw column numbers
        printw("   ");
        for (size_t x = 0; x < board_.getCols(); ++x) {
            printw("%2zu ", x + 1);
        }
        printw("\n");

        for (size_t y = 0; y < board_.getRows(); ++y) 
        {
            // Draw row number
            attron(A_BOLD);
            printw("%2zu ", y + 1);
            attroff(A_BOLD);

            for (size_t x = 0; x < board_.getCols(); ++x) 
            {
                const auto current = board_({y, x});
                
                // Highlight cell if mouse is over it
                if (mouseHover && mouseX == x && mouseY == y) {
                    attron(A_REVERSE);
                }

                switch (current.first)
                {
                case EMPTY:  printw(current.second ? "   " : "[ ]"); break;
                case ONE:    printw(current.second ? "[1]" : "[ ]"); break;
                case TWO:    printw(current.second ? "[2]" : "[ ]"); break;
                case THREE:  printw(current.second ? "[3]" : "[ ]"); break;
                case FOUR:   printw(current.second ? "[4]" : "[ ]"); break;
                case FIVE:   printw(current.second ? "[5]" : "[ ]"); break;
                case SIX:    printw(current.second ? "[6]" : "[ ]"); break;
                case SEVEN:  printw(current.second ? "[7]" : "[ ]"); break;
                case EIGHT:  printw(current.second ? "[8]" : "[ ]"); break;
                case MINE:   printw(current.second ? "[*]" : "[ ]"); break;
                case FLAG:   printw("[F]"); break;
                default:     printw("[?]"); break;
                }

                if (mouseX == x && mouseY == y) {
                    attroff(A_REVERSE);
                }
            }
            printw("\n");
        }
        
        // Draw instructions
        printw("\nLeft-click: Open cell | Right-click: Place flag\n");
        printw("Press 'q' to quit\n");
        refresh();
    }
    
    void 
    Game::generateMines()
    {
        const size_t area = board_.getCols() * board_.getRows();
        mines_count_ = area * 17 / 100; 
        if (mines_count_ < 1) { mines_count_ = 1; }  
        
        std::vector<size_t> positions(area);
        std::iota(positions.begin(), positions.end(), 0);
        std::shuffle(positions.begin(), positions.end(), std::mt19937(std::random_device()()));
        
        for (size_t i = 0; i < mines_count_; ++i)
        {
            placeBomb(positions[i]);
        }
    }

    void 
    Game::placeBomb(const size_t pos)
    {
        const size_t x = pos % board_.getCols();
        const size_t y = pos / board_.getCols();
        board_({y, x}) = {BoardElements::MINE, false};
        
        for (int dy = -1; dy <= 1; ++dy) 
        {
            for (int dx = -1; dx <= 1; ++dx) 
            {
                if (dx == 0 && dy == 0) continue;
                
                const size_t nx = x + dx;
                const size_t ny = y + dy;
                
                if (nx < board_.getCols() && ny < board_.getRows()) 
                {
                    auto& cell = board_({ny, nx});
                    if (cell.first != BoardElements::MINE) 
                    {
                        cell.first = BoardElements{int(cell.first) + 1};
                    }
                }
            }
        }
    }

    void 
    Game::openCell(const Coordinate::Coordinate& coord)
    {
        auto& cell = board_(coord);
        if (cell.first == BoardElements::FLAG) { return; }
        
        cell.second = true;
        
        if (cell.first == BoardElements::MINE) 
        {
            game_over_ = true;
            revealAllMines();
        }
        else if (cell.first == BoardElements::EMPTY) 
        {
            openEmptysFrom(coord);
        }
    }

    void 
    Game::openEmptysFrom(const Coordinate::Coordinate& coord)
    {
        std::queue<Coordinate::Coordinate> toProcess;
        toProcess.push(coord);

        while (!toProcess.empty())
        {
            auto current = toProcess.front();
            toProcess.pop();

            // Boundary check
            if (current.y >= board_.getRows() || current.x >= board_.getCols()) { continue; }

            auto& cell = board_(current);
            
            // Skip if already revealed or flagged
            if (cell.second || cell.first == BoardElements::FLAG) { continue; }

            // Reveal the cell
            cell.second = true;

            // Stop if this is a numbered cell
            if (cell.first != BoardElements::EMPTY) { continue; }

            // Process all 8 neighbors
            for (int dy = -1; dy <= 1; ++dy) 
            {
                for (int dx = -1; dx <= 1; ++dx) 
                {
                    if (dx == 0 && dy == 0) { continue; }
                    if (int(current.x) + dx < 0 || int(current.y) + dy < 0) { continue; }
                    size_t nx = current.x + dx;
                    size_t ny = current.y + dy;
                    
                    if (nx < board_.getCols() && ny < board_.getRows() && board_({ny, nx}).first != BoardElements::MINE) 
                    {
                        toProcess.push({ny, nx});
                    }
                }
            }
        }
    }

    void 
    Game::handleInput()
    {
        MEVENT event;
        int ch = getch();
        
        switch (ch) {
            case 'q': game_over_ = true; break;                
            case KEY_MOUSE:
                if (getmouse(&event) == OK) {
                    mouseHover = false;
                    
                    // Convert screen coordinates to board coordinates
                    const int boardStartX = 3;
                    const int boardStartY = 3; 
                    const int cellWidth = 3; 
                    
                    // Check if mouse is within board bounds
                    if (event.y >= boardStartY && event.x >= boardStartX) 
                    {
                        size_t potentialX = (event.x - boardStartX) / cellWidth;
                        size_t potentialY = event.y - boardStartY;
                    
                        // Validate coordinates
                        if (potentialX < board_.getCols() && 
                            potentialY < board_.getRows()) 
                        {
                            mouseX = potentialX;
                            mouseY = potentialY;
                            mouseHover = true;
                            
                            if (event.bstate & BUTTON1_CLICKED) { openCell({mouseY, mouseX}); }
                            else if (event.bstate & BUTTON3_CLICKED) 
                            {
                                auto& cell = board_({mouseY, mouseX});
                                if (!cell.second && cell.first != BoardElements::FLAG) {
                                    cell.first = BoardElements::FLAG;
                                    flags_placed_++;
                                } else if (cell.first == BoardElements::FLAG) {
                                    cell.first = BoardElements::EMPTY;
                                    flags_placed_--;
                                }
                            }
                        }
                    }
                }
                break;
        }
    }

    void 
    Game::start()
    {
        generateMines();
        
        while (!game_over_) {
            drawBoard();
            handleInput();
            if (checkWin()) { break; }
        }
            
        // Game over screen
        
        clear();
        if (checkWin()) { printw("Congratulations! You won!\n"); } 
        else { printw("Game Over! You hit a mine!\n"); }
        
        printw("Press any key to exit...");
        refresh();
        getch();
        
        endwin(); // Clean up ncurses
    }
    
    bool 
    Game::checkWin() const
    {
        for (size_t y = 0; y < board_.getRows(); ++y) 
        {
            for (size_t x = 0; x < board_.getCols(); ++x) 
            {
                const auto& cell = board_({y, x});
                if (cell.first != MINE && !cell.second) { return false; }
            }
        }
        
        return true;
    }

    void 
    Game::revealAllMines() 
    {
        for (size_t y = 0; y < board_.getRows(); ++y) 
        {
            for (size_t x = 0; x < board_.getCols(); ++x) 
            {
                if (board_({y, x}).first == BoardElements::MINE) 
                {
                    board_({y, x}).second = true;
                }
            }
        }
    }
}