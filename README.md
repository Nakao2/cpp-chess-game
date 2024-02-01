# cpp-chess-game

A simple simulation of a chess board that contains classic game of chess pieces.
Board can have any dimensions including classic 8x8. Pieces are allowed to move according to common chess rules.
Supports adding new pieces to the board at any time as well as removing them and emptying the board.
Built with potential for expansion in functions/capabilities in mind.

Also included:

1) board_visualizer.h
   Reads from an object of type Chess(board), and outputs a board representation in the console window.

2) board_state_container.h
   If given an object of type Chess, saves its' condition in std::deque container.
   Currently supports reading from a container as well as methods such as GetSize() and Clear().

3) cpu_opponent.h
   An object that must be tied to a game(board) and know what pieces it plays (White or Black).
   Generates moves randomly and updates the board. Does not have a strategy or follow any tactics,
   aside from being able to prioritize agressive moves(capturing a piece) over non-agressive.


Known bugs:
  1) cpu_opponent.h
     Method AgrMovePiece() sometimes ignores a possible capture move.


To do list (not sorted in priority):
  1) Add ability of a pawn to move two tiles ahead, if it hasn't been moved before.
  2) Add castling(chess move).
  3) Add a class derived from Chess that includes a BoardStateContainer as a feature.
