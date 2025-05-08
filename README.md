## Project Documentation

### Games Created in the Project
This project includes a collection of games implemented in C/C++. Each game is designed to run in a terminal environment and utilizes the `ncurses` library for rendering the user interface. The games are:

1. **[Snake](https://en.wikipedia.org/wiki/Snake_(video_game_genre))** - A classic game where the player controls a snake to collect food while avoiding collisions with the walls and itself.
2. **[Minesweeper](https://en.wikipedia.org/wiki/Minesweeper_(video_game))** - A puzzle game where the player uncovers tiles on a grid, avoiding hidden mines while using numerical clues to locate them.
3. **[Checkers](https://en.wikipedia.org/wiki/Draughts)** - A strategy board game where players move pieces diagonally to capture the opponent's pieces and aim to dominate the board.

### Running the Project on Linux
To ensure the project works correctly on a Linux system, follow these steps:

1. **Install Required Dependencies**:
   - The project relies on the `ncurses` library for terminal-based graphics. You need to install it before compiling the project.
   - Use the following command to install `ncurses`:
     ```bash
     sudo apt-get update
     sudo apt-get install libncurses5-dev libncursesw5-dev
     ```

2. **Compile the Project**:
   - Use a C/C++ compiler such as `gcc` or `g++` to compile the source code (use command `make`):
    ```bash
     make
     ```

3. **Run the Games**:
   - After successful compilation, you can run the games by executing the compiled binary:
     ```bash
     ./builds/debug/name_game
     ```

### Troubleshooting
- If you encounter errors related to `ncurses.h` not being found, ensure that the `libncurses5-dev` and `libncursesw5-dev` packages are installed correctly.
- Verify that the include paths in your source code are correct:
  ```c
  #include <ncurses.h>
  ```
- If the issue persists, try reinstalling the `ncurses` library:
  ```bash
  sudo apt-get install --reinstall libncurses5-dev libncursesw5-dev
  ```

### Additional Notes
- This project is designed to work in a terminal environment. Ensure you are running it in a compatible terminal emulator.
- For any additional dependencies or configurations, refer to the project's README file or documentation