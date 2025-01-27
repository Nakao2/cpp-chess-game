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

4) chess_history.h

Class derived form Chess. Added BoardStateContainer as a member.
Writes into it with every successful move.

5) chess_engine.h

Contains an algorithm that searches all possible moves at a certain depth and
chooses the best move by piece value gained.

Missing classic chess features:

1) While a checkmate makes any further moves impossible, 
   the user or cpu player are not notified of it.
