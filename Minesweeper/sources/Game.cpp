#include "../headers/Game.hpp"
#include <cstdlib>
#include <iomanip>
#include <numeric>
#include <ncurses.h>
#include <queue>
#include <unordered_set>

namespace SamHovhannisyan::MinesweeperGame
{
    size_t mouseX = 0;
    size_t mouseY = 0;
    bool mouseHover = false;

    Game::Game(const size_t width, const size_t height)
        : board_(width, height)
        , first_click_(true)
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
                board_({x, y}) = {BoardElements::EMPTY, false};
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
                const auto current = board_({x, y});
                
                // Highlight cell if mouse is over it
                if (mouseHover && mouseX == x && mouseY == y) {
                    attron(A_REVERSE);
                }

                if (getFlag({x, y}) != flags_.end()) 
                {
                    printw("[F]");
                    if (mouseX == x && mouseY == y) { attroff(A_REVERSE); }
                    continue;
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
    Game::generateMines(const Coordinate& coord)
    {
        if (coord == Coordinate(board_.getCols(), board_.getRows())) { return; }
        first_click_ = false;

        const size_t area = board_.getCols() * board_.getRows();
        mines_count_ = area * 17 / 100; 
        if (mines_count_ < 1) { mines_count_ = 1; }  
        flags_.reserve(mines_count_);
        
        std::vector<size_t> positions(area);
        std::iota(positions.begin(), positions.end(), 0);

        std::unordered_set<size_t> exclude_positions;
    
        // Add the clicked position
        exclude_positions.insert(coord.y * board_.getCols() + coord.x);

        // Add all 8 neighboring positions
        for (int dy = -2; dy <= 2; ++dy) 
        {
            for (int dx = -2; dx <= 2; ++dx) 
            {
                if (dx == 0 && dy == 0)  { continue; }
                
                int ny = coord.y + dy;
                int nx = coord.x + dx;
                
                if (ny >= 0 && ny < static_cast<int>(board_.getRows()) && 
                    nx >= 0 && nx < static_cast<int>(board_.getCols())) 
                {
                    exclude_positions.insert(ny * board_.getCols() + nx);
                }
            }
        }

        // Remove excluded positions from the possible mine positions
        positions.erase(
            std::remove_if(positions.begin(), positions.end(),
                [&exclude_positions](size_t pos) {
                    return exclude_positions.find(pos) != exclude_positions.end();
                }),
            positions.end()
        ); 

        std::shuffle(positions.begin(), positions.end(), std::mt19937(std::random_device()()));
        
        for (size_t i = 0; i < mines_count_; ++i) { placeBomb(positions[i]); }
    }

    void 
    Game::placeBomb(const size_t pos)
    {
        const size_t x = pos % board_.getCols();
        const size_t y = pos / board_.getCols();
        board_({x, y}) = {BoardElements::MINE, false};
        
        for (int dy = -1; dy <= 1; ++dy) 
        {
            for (int dx = -1; dx <= 1; ++dx) 
            {
                if (dx == 0 && dy == 0) continue;
                
                const size_t nx = x + dx;
                const size_t ny = y + dy;
                
                if (nx < board_.getCols() && ny < board_.getRows()) 
                {
                    auto& cell = board_({nx, ny});
                    if (cell.first != BoardElements::MINE) 
                    {
                        cell.first = BoardElements{int(cell.first) + 1};
                    }
                }
            }
        }
    }

    void 
    Game::openCell(const Coordinate& coord)
    {
        auto& cell = board_(coord);
        if (getFlag(coord) != flags_.end()) { return; }
        
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
    Game::openEmptysFrom(const Coordinate& coord)
    {
        std::queue<Coordinate> to_open;
        to_open.push(coord);
        
        while (!to_open.empty()) 
        {
            const auto current = to_open.front();
            to_open.pop();
            
            for (int dy = -1; dy <= 1; ++dy) 
            {
                for (int dx = -1; dx <= 1; ++dx) 
                {
                    if (dx == 0 && dy == 0) { continue; }
                    
                    const size_t nx = current.x + dx;
                    const size_t ny = current.y + dy;
                    
                    if (nx >= board_.getCols() || ny >= board_.getRows()) { continue; }
                    
                    auto& neighbor = board_({nx, ny});
                    if (neighbor.second) { continue; }
                    
                    neighbor.second = true;
                    if (neighbor.first == BoardElements::EMPTY) 
                    {
                        to_open.push({nx, ny});
                    }
                }
            }
        }
    }

    const typename Game::Coordinate
    Game::handleInput()
    {
        MEVENT event;
        int ch = getch();
        
        switch (ch) 
        {
        case 'q': game_over_ = true; return Coordinate(board_.getCols(), board_.getRows());                
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
                        
                        if (event.bstate & BUTTON1_CLICKED) { return Coordinate(mouseX, mouseY); }
                        else if (event.bstate & BUTTON3_CLICKED && !first_click_) 
                        {
                            placeRemoveFlag({mouseX, mouseY});
                        }
                    }
                }
            }
        }

        return Coordinate(board_.getCols(), board_.getRows());
    }

    void
    Game::placeRemoveFlag(const Coordinate& coord)
    {
        auto& cell = board_({coord.x, coord.y});
        const auto& flag = getFlag(coord);
        if (cell.second) { return; }
        if (!cell.second && flag == flags_.end()) 
        {
            flags_.push_back(coord);
            ++flags_placed_;
        }
        else if (flag != flags_.end()) 
        {
            flags_.erase(flag);
            --flags_placed_;
        }
    }

    std::vector<SamHovhannisyan::MinesweeperGame::Game::Coordinate>::const_iterator
    Game::getFlag(const Coordinate& coord) const
    {
        return std::find(flags_.begin(), flags_.end(), coord);
    }

    void 
    Game::start()
    {
        while (!game_over_) 
        {
            drawBoard();
            Coordinate coord = handleInput();
            
            // Check for quit
            if (coord == Coordinate(board_.getCols(), board_.getRows())) { continue; }
            
            if (first_click_) 
            {
                generateMines(coord);
                openCell(coord);  // Open the first clicked cell
                first_click_ = false;
            } 
            else { openCell(coord); }
            
            if (checkWin()) { game_over_ = true; }
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
                const auto& cell = board_({x, y});
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
                if (board_({x, y}).first == BoardElements::MINE) 
                {
                    board_({x, y}).second = true;
                }
            }
        }
    }
}
