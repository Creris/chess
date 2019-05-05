# C++ Chess
Chess made in C++ as a fun side project.

## TODO
* Integrate rules into the game
* CLI to make it playable
* Rules left to implement:
  * Check
  * Checkmate
  * Castling
  * En Passant
  * 50 move rule
  * 3-fold repetition

## Compilation
If you have Visual studio, you can just put all cpp and hpp files inside this project into it, and compile.

If you use gcc or clang, you have to wait for a makefile, or compile this manually(`gcc *.cpp pieces/*.cpp -o chess` should work).

## CLI design

### Code description

BC - Board code, such as B1, A1, A7 etc.
      
PC - Piece code, K, Q, R, B, N(Knight), P.
     Piece code can only be used if the piece can be uniquely identified, such as last of the piece remaining.
     
### Commands
* select PC/BC
  * Selects a piece by its piece code, or by the position on the board.
  * When a piece is selected, it will automatically redraw the board with information about where it can move.

* move [PC/BC] BC
  * Try to move a piece to given board position.
  * When only one parameter is provided, it is assumed that we are moving the selected piece.
  * When two parameters are provided, its semantically equivalent to `select PC/BC` followed by `move BC`.

* turn L [U]
  * Shows information about turn number L.
  * When optional U parameter is provided, shows all turns between L and U.

* info BC
  * Informs the user about what is on the board at given position.

* help
* commands
  * Prints information about usable commands
  
* restart
  * Restarts the match
  
## Plans for future
* 2D Graphics
* Audio
* AI using Monte-Carlo algorithm
* Multiplayer?
