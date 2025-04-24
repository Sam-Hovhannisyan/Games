#include "../headers/Game.hpp"

#include <locale.h>
#include <cassert>
#include <ncursesw/ncurses.h>

namespace SamHovhannisyan::Checkers
{
    Game::Game()
        : game_over_(false)
        , player_turn_(true)
        , board_(8, 8)
        , players_pieces_({12, 12})
    {}

    void
    Game::start() 
    {
        setlocale(LC_ALL, "");
        // Initialize ncurses
        initscr();
        // Disable line buffering
        cbreak();               
        // Enable special keys
        keypad(stdscr, TRUE);   

        generateDefaultBoard();

        while (!game_over_) {
            drawBoard();
            handleInput();
            changePlayer();
            
            // Check game status after each move
            game_over_ = isWin() || isDraw();
        }

        // Game over screen
        clear();
        if (isWin()) { 
            printw("Congratulations! %s won!\n", player_turn_ ? "Black" : "White"); 
        }
        else if (isDraw()) { 
            printw("Draw!\n");
        }
        printw("Press any key to exit...");
        refresh();
        getch();
        
        // Clean up ncurses
        endwin();
    }

    void
    Game::generateDefaultBoard() 
    {
        size_t y = 0;
        for (; y < 3; ++y) {
            for (size_t x = (y % 2); x < 8; x += 2) {
                board_({x, y}).value = BoardElements::WHITE;
            }
        }

        for (; y < 5; ++y) {
            for (size_t x = 0; x < 8; ++x) {
                board_({x, y}).value = BoardElements::EMPTY;
            }
        }

        for (; y < 8; ++y) {
            for (size_t x = (y % 2); x < 8; x += 2) {
                board_({x, y}).value = BoardElements::BLACK;
            }
        }
    }

    void
    Game::drawBoard() const
    {
        // Clear the screen
        clear(); 
        printw("\tBlacks: %d | Whites: %d\n", players_pieces_.first, players_pieces_.second);
        // Print column numbers
        printw("/x");
        for (size_t x = 0; x < board_.getCols(); ++x) {
            printw(" %ld", x);
        }
        printw("\n");
        
        // Print top border
        printw("y ");
        for (size_t x = 0; x < board_.getCols() * 2 + 1; ++x) {
            printw("_");
        }
        printw("\n");
        
        for (size_t y = 0; y < board_.getRows(); ++y) {
            // Print row number
            printw("%ld |", y);
            
            for (size_t x = 0; x < board_.getCols(); ++x) {
                const Piece& piece = getPiece({x, y});
                
                // Print the piece or empty space
                switch (piece.value) 
                {
                case BoardElements::EMPTY:      printw(" ");            break;
                case BoardElements::BLACK:      addwstr(L"\U000026C0"); break;
                case BoardElements::WHITE:      addwstr(L"\U000026C2"); break;
                case BoardElements::BLACK_KING: addwstr(L"\U000026C1"); break;
                case BoardElements::WHITE_KING: addwstr(L"\U000026C3"); break;
                }
                
                // Print cell separator
                printw("|");
            }
            
            // Print bottom border for the row
            printw("\n  ");
            for (size_t x = 0; x < board_.getCols() * 2 + 1; ++x) {
                printw("-");
            }
            printw("\n");
        }
        
        // Print current player turn
        printw("Current turn: ");
        player_turn_ ? addwstr(L"Black (\U000026C0)\n") : addwstr(L"White (\U000026C2)\n");
        printw("Instructions: Enter move as 'fromX fromY toX toY' (e.g., '1 2 2 3')");
        refresh();
    }

    void
    Game::handleInput()
    {
        size_t fromX, fromY, toX, toY;
        
        // Get input until a valid move is entered
        while (true) {
            printw("\nEnter your move: ");
            refresh();
            
            // Read input
            scanw("%ld %ld %ld %ld", &fromX, &fromY, &toX, &toY);
            
            // Validate coordinates are within bounds
            if (fromX >= board_.getCols() ||
                fromY >= board_.getRows() ||
                toX   >= board_.getCols() ||
                toY   >= board_.getRows()) 
            {
                printw("Invalid coordinates. Try again.\n");
                continue;
            }
            
            Coordinate from{fromX, fromY};
            Coordinate to{toX, toY};
            
            // Check if the move is legal
            if (isLegalMove(from, to)) {
                movePiece(from, to);
                break;
            } else {
                printw("Illegal move. Try again.\n");
            }
        }
    }

    void
    Game::changePlayer()
    {
        player_turn_ = !player_turn_;
    }

    void
    Game::promote(const Coordinate& coord)
    {
        Piece& piece = getPiece(coord);
        if (piece.value == BoardElements::EMPTY) { return; }
        if (int(piece.value) > 2) { return; }
        piece.value = BoardElements(int(piece.value) + 2);
    }

    typename Game::Piece&
    Game::getPiece(const Coordinate& coord)
    {
        return board_(coord);
    }

    const typename Game::Piece&
    Game::getPiece(const Coordinate& coord) const
    {
        return board_(coord);
    }

    bool 
    Game::isLegalMove(const Coordinate& from, const Coordinate& to) const
    {
        // Check if moving to same position
        if (from == to) { return false; }

        // Validate coordinates
        if (from.x >= board_.getCols() || from.y >= board_.getRows() ||
              to.x >= board_.getCols() ||   to.y >= board_.getRows()) 
        { return false; }

        const Piece& piece  = getPiece(from);
        const Piece& target = getPiece(to);

        // Check if source has a piece and target is empty
        if (piece.value == BoardElements::EMPTY || target.value != BoardElements::EMPTY) 
        { return false; }

        // Calculate movement direction and distance
        const int dx = static_cast<int>(to.x) - static_cast<int>(from.x);
        const int dy = static_cast<int>(to.y) - static_cast<int>(from.y);
        const int absDx = abs(dx);
        const int absDy = abs(dy);

        // Must move diagonally
        if (absDx != absDy) { return false; }

        // Check piece type and movement rules
        switch (piece.value) 
        {
        case BoardElements::BLACK: // Regular black piece
        
            // Capture move (dx = ±2)
            if (absDx == 2 && dy == -2) {
                const int midX = from.x + dx / 2;
                const int midY = from.y + dy / 2;
                const Piece& midPiece = getPiece({static_cast<size_t>(midX), static_cast<size_t>(midY)});
                return (midPiece.value == BoardElements::WHITE || midPiece.value == BoardElements::WHITE_KING);
            }
            if (dy != -1 )   { return false; } // Must move upward
            if (absDx == 1) { return  true; } // Regular move

            break;

        case BoardElements::WHITE: // Regular white piece
            
            // Capture move (dx = ±2)
            if (absDx == 2 && dy == 2) {
                const int midX = from.x + dx / 2;
                const int midY = from.y + dy / 2;
                const Piece& midPiece = getPiece({static_cast<size_t>(midX), static_cast<size_t>(midY)});
                return (midPiece.value == BoardElements::BLACK || midPiece.value == BoardElements::BLACK_KING);
            }
            if (dy != 1)    { return false; } // Must move downward
            if (absDx == 1) { return  true; } // Regular move

            break;

        case BoardElements::BLACK_KING:  // Black king
        case BoardElements::WHITE_KING:  // White king
            if (absDx == 1) return true; // Regular king move
            
            // King capture move
            if (absDx == 2) {
                const int midX = from.x + dx / 2;
                const int midY = from.y + dy / 2;
                const Piece& midPiece = getPiece({static_cast<size_t>(midX), static_cast<size_t>(midY)});
                
                if (piece.value == BoardElements::BLACK_KING) {
                    return (midPiece.value == BoardElements::WHITE || midPiece.value == BoardElements::WHITE_KING);
                } else {
                    return (midPiece.value == BoardElements::BLACK || midPiece.value == BoardElements::BLACK_KING);
                }
            }
            break;
        default: return false;
        }

        return false;
    }

    bool
    Game::isWin() const
    {
        // Check if either player has no pieces left
        if (players_pieces_.first == 0 || players_pieces_.second == 0) {
            return true;
        }

        // Check if current player has no valid moves left
        bool hasValidMoves = false;
        for (size_t y = 0; y < board_.getRows(); ++y) {
            for (size_t x = 0; x < board_.getCols(); ++x) {
                const Piece& piece = getPiece({x, y});
                
                // Skip empty squares and opponent's pieces
                if (piece.value == BoardElements::EMPTY || 
                    ( player_turn_ && ((piece.value & BoardElements::WHITE) == BoardElements::WHITE)) ||
                    (!player_turn_ && ((piece.value & BoardElements::BLACK) == BoardElements::BLACK))) 
                { continue; }

                // Check all possible moves for this piece
                for (int dy = -2; dy <= 2; ++dy) {
                    for (int dx = -2; dx <= 2; ++dx) {
                        if (dx == 0 || dy == 0) continue; // Skip non-diagonal moves
                        
                        size_t newX = x + dx;
                        size_t newY = y + dy;
                        
                        if (newX >= board_.getCols() || newY >= board_.getRows()) continue;
                        
                        Coordinate from{x, y};
                        Coordinate to{newX, newY};
                        
                        if (isLegalMove(from, to)) {
                            hasValidMoves = true;
                            break;
                        }
                    }
                    if (hasValidMoves) break;
                }
                if (hasValidMoves) break;
            }
            if (hasValidMoves) break;
        }

        return !hasValidMoves;
    }

    bool
    Game::isDraw() const
    {
        // Check for 20 moves without capture or promotion (simplified rule)
        static int movesWithoutProgress = 0;
        static int lastBlackCount = players_pieces_.first;
        static int lastWhiteCount = players_pieces_.second;
        
        // If pieces were captured or promoted, reset counter
        if (players_pieces_.first != lastBlackCount || 
            players_pieces_.second != lastWhiteCount) {
            movesWithoutProgress = 0;
            lastBlackCount = players_pieces_.first;
            lastWhiteCount = players_pieces_.second;
            return false;
        }
        
        // Increment counter and check for draw
        if (++movesWithoutProgress >= 20) { return true; }
        
        // Check for insufficient material (just kings left)
        bool onlyKingsLeft = true;
        for (size_t y = 0; y < board_.getRows(); ++y) {
            for (size_t x = 0; x < board_.getCols(); ++x) {
                const Piece& piece = getPiece({x, y});
                if (piece.value == BoardElements::EMPTY) continue;
                if (piece.value == BoardElements::WHITE || piece.value == BoardElements::BLACK) {
                    onlyKingsLeft = false;
                    break;
                }
            }
            if (!onlyKingsLeft) break;
        }
        
        if (onlyKingsLeft && players_pieces_.first == 1 && players_pieces_.second == 1) {
            return true; // Only two kings left - automatic draw
        }
        
        return false;
    }

    void
    Game::movePiece(const Coordinate& from, const Coordinate& to)
    {
        Piece& piece  = getPiece(from);
        Piece& target = getPiece(to);
        
        // Calculate movement direction and distance
        const int dx = static_cast<int>(to.x) - static_cast<int>(from.x);
        const int dy = static_cast<int>(to.y) - static_cast<int>(from.y);
        const int absDx = abs(dx);
        
        // Handle capture
        if (absDx == 2) {  // This is a capture move
            const size_t capturedX = from.x + dx / 2;
            const size_t capturedY = from.y + dy / 2;
            takePiece({capturedX, capturedY});
        }
        
        // Move the piece
        target = piece;
        piece.value = BoardElements::EMPTY;
        
        // Check for promotion
        if ((target.value == BoardElements::WHITE && to.y == 0) ||
            (target.value == BoardElements::BLACK && to.y == board_.getRows() - 1)) {
            promote(to);
        }
    }

    void
    Game::takePiece(const Coordinate& coord)
    {
        Piece& piece = getPiece(coord);
        if (piece.value == BoardElements::EMPTY) { return; }
        
        // Update player pieces count
        if ((piece.value & BoardElements::WHITE) == BoardElements::WHITE) {
            players_pieces_.second--;
        } else {
            players_pieces_.first--;
        }

        // Remove the piece
        piece.value = BoardElements::EMPTY;
    }

}
