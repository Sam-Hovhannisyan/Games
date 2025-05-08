#include "../headers/Game.hpp"

#include <locale.h>
#include <cassert>
#include <ncursesw/ncurses.h>

namespace SamHovhannisyan::Checkers
{
    Game::Game()
        : game_over_(false)
        , player_turn_(true)
        , is_capture_available_(false)
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
            bool flag = true;
            while (flag) {
                drawBoard();
                flag = handleInput();
            }
            changePlayer();
            
            // Check game status after each move
            game_over_ = isWin() || isDraw();
        }

        // Game over screen
        clear();
        if (isWin()) { 
            printw("Congratulations! %s won!\n", !player_turn_ ? "Black" : "White"); 
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
        for (size_t y = 0; y < 3; ++y) {
            for (size_t x = (y % 2); x < 8; x += 2) {
                board_({x, y}).value = BoardElements::WHITE;
            }
        }
        
        for (size_t y = 5; y < 8; ++y) {
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

    bool
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
                return movePiece(from, to);
            } else {
                printw("Illegal move. Try again.\n");
            }
        }
        return false; // This line will never be reached
    }

    void
    Game::changePlayer()
    {
        player_turn_ = !player_turn_;
        is_capture_available_ = false;
    }

    void
    Game::promote(const Coordinate& coord)
    {
        Piece& piece = getPiece(coord);
        if (piece.value == BoardElements::EMPTY) { return; }
        if (int(piece.value) > 2) { return; }
        if ((player_turn_  && coord.y == 0) ||
            (!player_turn_ && coord.y == board_.getRows() - 1)) 
        {
            piece.value = BoardElements(int(piece.value) + 2);
        }
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

    std::vector<typename Game::Coordinate>
    Game::getPieceCoordinates() const
    {
        std::vector<Coordinate> coordinates;
        for (size_t y = 0; y < board_.getRows(); ++y) {
            for (size_t x = 0; x < board_.getCols(); ++x) {
                if (board_({x, y}).value == BoardElements::EMPTY) { continue; }
                if (player_turn_  && (board_({x, y}).value == BoardElements::WHITE || board_({x, y}).value == BoardElements::WHITE_KING)) { continue; }
                if (!player_turn_ && (board_({x, y}).value == BoardElements::BLACK || board_({x, y}).value == BoardElements::BLACK_KING)) { continue; }
                coordinates.push_back({x, y});
            }
        }
        return coordinates;
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
        
        if (player_turn_  && !(piece.value == BoardElements::BLACK || piece.value == BoardElements::BLACK_KING)) { return false; }
        if (!player_turn_ && !(piece.value == BoardElements::WHITE || piece.value == BoardElements::WHITE_KING)) { return false; }

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
        case BoardElements::EMPTY: return false;

        case BoardElements::BLACK: // Regular black piece
        
            // Capture move (dx = ±2)
            if (absDx == 2 && dy == -2) {
                const int midX = from.x + dx / 2;
                const int midY = from.y + dy / 2;
                const Piece& midPiece = getPiece({static_cast<size_t>(midX), static_cast<size_t>(midY)});
                return (midPiece.value == BoardElements::WHITE || midPiece.value == BoardElements::WHITE_KING);
            }
            if (dy != -1)   { return false; } // Must move upward
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
            int opponentPieceCount = 0;
            
            const int dxStep = dx / absDx;
            const int dyStep = dy / absDy;

            size_t fromX = from.x;
            size_t fromY = from.y;
            
            while (fromX != to.x && fromY != to.y && opponentPieceCount < 2) {
                fromX += dxStep;
                fromY += dyStep;

                const Piece& current = getPiece({fromX, fromY}); 
                if ((!player_turn_ && (current.value == BoardElements::WHITE || current.value == BoardElements::WHITE_KING)) ||
                    (player_turn_  && (current.value == BoardElements::BLACK || current.value == BoardElements::BLACK_KING))) 
                { return false; }

                if (isOpponentsPiece(current)) {
                    ++opponentPieceCount;
                }
            }
            return opponentPieceCount < 2;
        }

        return false;
    }

    bool
    Game::isWin() const
    {
        // Check if either player has no pieces left
        if (players_pieces_.first == 0 || players_pieces_.second == 0) 
        { return true; }

        auto pieces = getPieceCoordinates();
        for (const auto& piece : pieces) {
            if (hasAvailableMove(piece)) 
            { return false; }
        }

        return true;
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
        
        if (onlyKingsLeft && players_pieces_.first == 1 && players_pieces_.second == 1) 
        { return true; }// Only two kings left - automatic draw
        
        return false;
    }

    bool
    Game::movePiece(const Coordinate& from, const Coordinate& to)
    {
        const Piece& piece = getPiece(from);
        if (piece.value == BoardElements::EMPTY) 
        { return false; }

        if (piece.value == BoardElements::BLACK || piece.value == BoardElements::WHITE) 
        { return movePieceMan(from, to); }

        if (piece.value == BoardElements::BLACK_KING || piece.value == BoardElements::WHITE_KING) 
        { return movePieceKing(from, to); }

        return false;
    }

    bool
    Game::movePieceMan(const Coordinate& from, const Coordinate& to)
    {
        Piece& piece  = getPiece(from);
        Piece& target = getPiece(to);

        // Calculate movement direction and distance
        const int dx = static_cast<int>(to.x) - static_cast<int>(from.x);
        const int dy = static_cast<int>(to.y) - static_cast<int>(from.y);
        const int absDx = abs(dx);

        if (absDx == 1) {
            if (is_capture_available_) { return true; }
            Coordinate coord;
            if (isFreePieceAvailable(coord)) {
                takePiece(coord);
                target = piece;
                piece.value = BoardElements::EMPTY;
                promote(to);
                return false;
            }
        }

        if (absDx == 2) { // This is a capture move
            const size_t capturedX = from.x + dx / 2;
            const size_t capturedY = from.y + dy / 2;
            target = piece;
            is_capture_available_ = isFreePieceAround(to);
            target.value = BoardElements::EMPTY;
            takePiece({capturedX, capturedY});
        }
        
        // Move the piece
        target = piece;
        piece.value = BoardElements::EMPTY;
        
        promote(to);
        return is_capture_available_;
    }

    bool
    Game::movePieceKing(const Coordinate& from, const Coordinate& to)
    {
        Piece& piece  = getPiece(from);
        Piece& target = getPiece(to);

        // Calculate movement direction and distance
        const int dx = static_cast<int>(to.x) - static_cast<int>(from.x);
        const int dy = static_cast<int>(to.y) - static_cast<int>(from.y);
        const int absDx = abs(dx);
        const int absDy = abs(dy);

        const int dxStep = dx / absDx;
        const int dyStep = dy / absDy;

        size_t fromX = from.x;
        size_t fromY = from.y;

        while (fromX != to.x && fromY != to.y) {
            fromX += dxStep;
            fromY += dyStep;

            const Piece& current = getPiece({fromX, fromY});

            if (isOpponentsPiece(current)) {
                takePiece({fromX, fromY});
                target = piece;
                is_capture_available_ = isFreePieceAround(to);
                target.value = BoardElements::EMPTY;
                break;
            }
        }

        Coordinate coord;
        if (fromX == to.x && fromY == to.y && isFreePieceAvailable(coord)) {
            takePiece(coord);
            target = piece;
            piece.value = BoardElements::EMPTY;
            return false;
        }
        
        target = piece;
        piece.value = BoardElements::EMPTY;

        return is_capture_available_;
    }

    void
    Game::takePiece(const Coordinate& coord)
    {
        Piece& piece = getPiece(coord);
        if (piece.value == BoardElements::EMPTY) 
        { return; }
        
        // Update player pieces count
        if ((piece.value & BoardElements::WHITE) == BoardElements::WHITE) {
            players_pieces_.second--;
        } else {
            players_pieces_.first--;
        }

        // Remove the piece
        piece.value = BoardElements::EMPTY;
    }

    bool 
    Game::isOpponentsPiece(const Piece& piece) const
    {
        if (piece.value == BoardElements::EMPTY) { return false; }
        return (player_turn_  && (piece.value == BoardElements::WHITE || piece.value == BoardElements::WHITE_KING)) ||
               (!player_turn_ && (piece.value == BoardElements::BLACK || piece.value == BoardElements::BLACK_KING));
    }

    bool
    Game::isFreePieceAround(const Coordinate& coord) const
    {
        const Piece& piece = getPiece(coord);
        if (piece.value == BoardElements::EMPTY) 
        { return false; }

        if (piece.value == BoardElements::BLACK || 
            piece.value == BoardElements::WHITE) 
        { return isFreePieceAroundMan(coord); }

        if (piece.value == BoardElements::BLACK_KING || 
            piece.value == BoardElements::WHITE_KING) 
        { return isFreePieceAroundKing(coord); }

        return false;
    }

    bool
    Game::isFreePieceAroundMan(const Coordinate& coord) const
    {
        // Check if current coordinate is within bounds
        if (coord.x >= board_.getCols() || coord.y >= board_.getRows()) 
        { return false; }

        const Piece& piece = getPiece(coord);
        if (piece.value == BoardElements::EMPTY) 
        { return false; }

        if (piece.value == BoardElements::WHITE) {
            // Check right diagonal
            if (coord.x + 2 < board_.getCols() && coord.y + 2 < board_.getRows()) {
                if (getPiece({coord.x + 1, coord.y + 1}).value == BoardElements::BLACK && 
                    getPiece({coord.x + 2, coord.y + 2}).value == BoardElements::EMPTY) {
                    return true;
                }
            }
            // Check left diagonal
            if (coord.x >= 2 && coord.y + 2 < board_.getRows()) {
                if (getPiece({coord.x - 1, coord.y + 1}).value == BoardElements::BLACK && 
                    getPiece({coord.x - 2, coord.y + 2}).value == BoardElements::EMPTY) {
                    return true;
                }
            }
            return false;
        }

        if (piece.value == BoardElements::BLACK) {
            // Check right diagonal
            if (coord.x + 2 < board_.getCols() && coord.y >= 2) {
                if (getPiece({coord.x + 1, coord.y - 1}).value == BoardElements::WHITE && 
                    getPiece({coord.x + 2, coord.y - 2}).value == BoardElements::EMPTY) {
                    return true;
                }
            }
            // Check left diagonal
            if (coord.x >= 2 && coord.y >= 2) {
                if (getPiece({coord.x - 1, coord.y - 1}).value == BoardElements::WHITE && 
                    getPiece({coord.x - 2, coord.y - 2}).value == BoardElements::EMPTY) {
                    return true;
                }
            }
            return false;
        }

        return false;
    }

    bool
    Game::isFreePieceAroundKing(const Coordinate& coord) const
    {
        const Piece& piece = getPiece(coord);
        if (piece.value == BoardElements::EMPTY) { return false; }

        const std::vector<std::pair<int, int>> directions = {
            {1, 1},   // Down-right
            {1, -1},  // Up-right
            {-1, 1},  // Down-left
            {-1, -1}  // Up-left
        };

        for (const auto& direction : directions) {
            Coordinate newCoord = coord;

            while (true) {
                newCoord.x += direction.first;
                newCoord.y += direction.second;

                // Check if the new coordinate is out of bounds
                if (newCoord.x >= board_.getCols() || newCoord.y >= board_.getRows()) 
                { break; }

                const Piece& current = getPiece(newCoord);
                if (isOpponentsPiece(current)) {
                    Coordinate nextCoord = {newCoord.x + direction.first, newCoord.y + direction.second};
                    if (nextCoord.x < board_.getCols() && nextCoord.y < board_.getRows() &&
                        getPiece(nextCoord).value == BoardElements::EMPTY) 
                    { return true; }
                } else if (current.value != BoardElements::EMPTY) {
                    break; // Stop if a non-empty piece blocks the path
                }
            }
        }
        
        return false;
    }

    bool
    Game::isFreePieceAvailable(Coordinate& coord) const
    {
        std::vector<Coordinate> coordinates = getPieceCoordinates();
        for (size_t i = 0; i < coordinates.size(); ++i) {
            const Piece& currentPiece = getPiece(coordinates[i]);
            if ((player_turn_ &&
                ((currentPiece.value & BoardElements::WHITE) == BoardElements::WHITE)) ||
                (!player_turn_ &&
                ((currentPiece.value & BoardElements::BLACK) == BoardElements::BLACK))) { continue; }
            if (isFreePieceAround(coordinates[i])) {
                coord = coordinates[i];
                return true;
            }
        }
        coord = {board_.getCols(), board_.getRows()};
        return false;
    }

    bool
    Game::hasAvailableMove(const Coordinate& coord) const
    {
        const Piece& piece = getPiece(coord);

        const std::vector<std::pair<int, int>> directions = {
            {1, 1},   // Down-right
            {1, -1},  // Up-right
            {-1, 1},  // Down-left
            {-1, -1}  // Up-left
        };

        switch (piece.value) 
        {
            case BoardElements::EMPTY: return false;

            case BoardElements::BLACK:
            case BoardElements::WHITE:
            {
                Coordinate newCoord(coord.x, coord.y + (piece.value == BoardElements::BLACK ? -1 : 1)); 
                // Check forward moves
                if (newCoord.y < board_.getRows()) {
                    if (newCoord.x + 1 < board_.getCols() && 
                        getPiece({newCoord.x + 1, newCoord.y}).value == BoardElements::EMPTY) 
                    { return true; }
                    // Check left diagonal
                    if (newCoord.x > 0 && 
                        getPiece({newCoord.x - 1, newCoord.y}).value == BoardElements::EMPTY) 
                    { return true; }
                }
                break;
            }
            
            case BoardElements::BLACK_KING:
            case BoardElements::WHITE_KING:
            {
                for (const auto& direction : directions) {
                    if (coord.x + direction.first >= board_.getCols() || coord.y + direction.second >= board_.getRows()) 
                    { continue; }
                    if (getPiece({coord.x + direction.first, coord.y + direction.second}).value == BoardElements::EMPTY)
                    { return true; }
                }
                break;
            }
        }

        return false;
    }
}
