#pragma once

#include <deque>
#include <tuple>

enum class ChessPiece {
	EMPTY,

	PAWN,
	ROOK,
	BISHOP,
	KNIGHT,
	QUEEN,
	KING
};

enum class ChessTeam {
	NEUTRAL,

	WHITE,
	BLACK
};

struct BoardTile {
	ChessPiece piece_type = ChessPiece::EMPTY;
	ChessTeam piece_team = ChessTeam::NEUTRAL;
	bool has_moved = false;                   // Important to know for pawn moves and potential for castling
};

class Chess {
public:

	// Board has matrix-like dimensions of (n x m), where an element of 1x1 board has coordinates (0, 0)
	Chess(int n, int m);

	// Classic game of chess piece setup
	Chess();

	// Creates a copy of a board state and stores it in newly allocated memory
	// array_ptr_ will be unique
	Chess(const Chess& source);

	// Copies board state. Previous state of *this is destroyed
	Chess& operator=(const Chess& source);

	virtual ~Chess() {
		CleanUp();
	}

	BoardTile LookUp(int row, int column) const;

	void FillBoardWith(const BoardTile& piece);

	void FillBoardWithPawns();

	void EmptyBoard();

	void PutPieceInPosition(const BoardTile& piece, int row, int column);

	// Gives all possible destination tiles on the board for a selected piece
	std::deque<std::pair<int, int>> GetPossibleDestTiles(int n_input, int m_input) const;

	// Does all the necessary checks, moves a piece and returns 'true'
	// Or does nothing and returns 'false' if the move is illegal
	virtual bool MovePiece(std::pair<int, int> input_pos, std::pair<int, int> dest_pos);

	std::pair<int, int> GetDimensions() const;

	ChessTeam WhoseMove() const;

private:
	BoardTile** array_ptr_;
	int rows_ = 0;
	int columns_ = 0;
	bool is_whites_move_ = true;

	bool CheckOutOfBounds(int row, int column) const;

	// Can't move OutOfBounds or EMPTY tile
	bool CheckValidPieceSelected(int n_input, int m_input) const;

	// Can't move black pieces at whites turn
	bool CheckCorrectTurnSequence(int n_input, int m_input) const;


	// Checks piece movement according to chess rules
	// Also does OutOfBounds check for destination tile
	// Expected to run after CheckValidPieceSelected()	
	bool CheckLegalPieceMove(int n_input, int m_input, int n_dest, int m_dest) const;


	// Collision with pieces in the path of movement
    // Expected to run after CheckValidPieceSelected() and CheckLegalPieceMove()
	bool CheckCollision(int n_input, int m_input, int n_dest, int m_dest) const;

	void CleanUp() {
		for (int i = 0; i < rows_; ++i) {
			delete[] array_ptr_[i];
		}
		delete[] array_ptr_;
		array_ptr_ = nullptr;
	}
};