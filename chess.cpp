#include "chess.h"

#include <algorithm>
#include <deque>
#include <iostream>
#include <new>
#include <cmath>

using namespace std;

constexpr int STANDART_BOARD_WIDTH = 8;
constexpr int STANDART_BOARD_LENGTH = 8;

// Board has matrix-like dimensions of (n x m), where an element of 1x1 board has coordinates (0, 0)
Chess::Chess(int n, int m) {
	try {
		BoardTile** row_ptr = new BoardTile * [n];
		rows_ = n;
		columns_ = m;
		array_ptr_ = row_ptr;
		for (int i = 0; i < n; ++i) {
			BoardTile* column_ptr = new BoardTile[m];
			row_ptr[i] = column_ptr;
		}
	}
	catch (const bad_alloc&) {
		CleanUp();
		throw;
	}
}

// Classic game of chess piece setup
Chess::Chess() : Chess(STANDART_BOARD_WIDTH, STANDART_BOARD_LENGTH) {
	for (int row = 0; row < rows_; row += 7) {                             // To avoid code duplication
		ChessTeam team = (row == 0) ? ChessTeam::BLACK : ChessTeam::WHITE;
		array_ptr_[row][0] = { ChessPiece::ROOK, team };
		array_ptr_[row][1] = { ChessPiece::KNIGHT, team };
		array_ptr_[row][2] = { ChessPiece::BISHOP, team };
		array_ptr_[row][3] = { ChessPiece::QUEEN, team };
		array_ptr_[row][4] = { ChessPiece::KING, team };
		array_ptr_[row][5] = { ChessPiece::BISHOP, team };
		array_ptr_[row][6] = { ChessPiece::KNIGHT, team };
		array_ptr_[row][7] = { ChessPiece::ROOK, team };
	}
	for (int column = 0; column < columns_; ++column) {
		array_ptr_[1][column] = { ChessPiece::PAWN, ChessTeam::BLACK };
	}
	for (int column = 0; column < columns_; ++column) {
		array_ptr_[6][column] = { ChessPiece::PAWN, ChessTeam::WHITE };
	}
}

// Creates a copy of a board state and stores it in newly allocated memory
// array_ptr_ will be unique
Chess::Chess(const Chess& source) : Chess(source.GetDimensions().first, source.GetDimensions().second) {
	for (int row = 0; row < rows_; ++row) {
		for (int column = 0; column < columns_; ++column) {
			const BoardTile& tile = source.LookUp(row, column);
			this->PutPieceInPosition(tile, row, column);
		}
	}
	this->is_whites_move_ = source.is_whites_move_;
}

// Copies board state. Previous state of *this is destroyed
Chess& Chess::operator=(const Chess& source) {
	if (&source == this) {
		return *this;
	}
	Chess copy(source);
	std::swap(this->array_ptr_, copy.array_ptr_);
	std::swap(this->rows_, copy.rows_);
	std::swap(this->columns_, copy.columns_);
	this->is_whites_move_ = copy.is_whites_move_;  // Is not used in 'copy' destructor, swap unecessary
	return *this;
}

BoardTile Chess::LookUp(int row, int column) const {
	if (!CheckOutOfBounds(row, column)) {
		return array_ptr_[row][column];
	}
	return {};
}

void Chess::FillBoardWith(const BoardTile& piece) {
	for (int i = 0; i < rows_; ++i) {
		for (int k = 0; k < columns_; ++k) {
			array_ptr_[i][k] = piece;
		}
	}
}

void Chess::FillBoardWithPawns() {
	FillBoardWith({ ChessPiece::PAWN, ChessTeam::WHITE });
}

void Chess::EmptyBoard() {
	FillBoardWith({ ChessPiece::EMPTY, ChessTeam::NEUTRAL });
}

void Chess::PutPieceInPosition(const BoardTile& piece, int row, int column) {
	if (!CheckOutOfBounds(row, column)) {
		array_ptr_[row][column] = piece;
	}
}

// Gives all possible destination tiles on the board for a selected piece
deque<pair<int, int>> Chess::GetPossibleDestTiles(int n_input, int m_input) const {
	deque<pair<int, int>> output;
	if (!CheckValidPieceSelected(n_input, m_input)) {
		return output;
	}
	for (int n = 0; n < rows_; ++n) {        // Brute force. Inefficient for large boards
		for (int m = 0; m < columns_; ++m) {
			if (CheckLegalPieceMove(n_input, m_input, n, m) && !CheckCollision(n_input, m_input, n, m)) {
				output.push_back({ n, m });
			}
		}
	}
	return output;
}

// Does all the necessary checks, moves a piece and returns 'true'
// Or does nothing and returns 'false' if the move is illegal
bool Chess::MovePiece(pair<int, int> input_pos, pair<int, int> dest_pos) {
	if (CheckValidPieceSelected(input_pos.first, input_pos.second) && CheckCorrectTurnSequence(input_pos.first, input_pos.second) &&
		CheckLegalPieceMove(input_pos.first, input_pos.second, dest_pos.first, dest_pos.second) &&
		!CheckCollision(input_pos.first, input_pos.second, dest_pos.first, dest_pos.second)) {

		array_ptr_[dest_pos.first][dest_pos.second] = array_ptr_[input_pos.first][input_pos.second];
		array_ptr_[input_pos.first][input_pos.second] = { ChessPiece::EMPTY, ChessTeam::NEUTRAL };
		is_whites_move_ = (is_whites_move_) ? 0 : 1;
		return true;
	}
	else {
		return false;
	}
}

pair<int, int> Chess::GetDimensions() const {
	return { rows_, columns_ };
}

ChessTeam Chess::WhoseMove() const {
	return (is_whites_move_) ? ChessTeam::WHITE : ChessTeam::BLACK;
}

inline bool Chess::CheckOutOfBounds(int row, int column) const {
	if ((row >= rows_) || (row < 0)) {
		return true;
	}
	if ((column >= columns_) || (column < 0)) {
		return true;
	}
	return false;
}

// Can't move OutOfBounds or EMPTY tile
bool Chess::CheckValidPieceSelected(int n_input, int m_input) const {
	if (!CheckOutOfBounds(n_input, m_input)) {
		return (array_ptr_[n_input][m_input].piece_type != ChessPiece::EMPTY);
	}
	cout << "Invalid piece selected" << endl;
	return false;
}

// Can't move black pieces at whites turn
bool Chess::CheckCorrectTurnSequence(int n_input, int m_input) const {
	if (array_ptr_[n_input][m_input].piece_team == ChessTeam::WHITE) {
		return is_whites_move_;
	}
	else {
		return !is_whites_move_;
	}
}

// Checks piece movement according to chess rules
// Also does OutOfBounds check for destination tile
// Expected to run after CheckValidPieceSelected()	
bool Chess::CheckLegalPieceMove(int n_input, int m_input, int n_dest, int m_dest) const {
	if (n_input == n_dest && m_input == m_dest) {
		return false;
	}
	if (CheckOutOfBounds(n_dest, m_dest)) {
		return false;
	}
	const BoardTile& piece = array_ptr_[n_input][m_input];
	switch (piece.piece_type) {
	default:
		std::cout << "No chess pieces at that position"s << endl;
		return false;
	case ChessPiece::KNIGHT:
	{
		int n_abs_dif = std::abs(n_input - n_dest);
		if (n_abs_dif == 2 || n_abs_dif == 1) {
			if (n_abs_dif + std::abs(m_input - m_dest) == 3) {
				return true;
			}
		}
		return false;
	}
	case ChessPiece::BISHOP:
		if (std::abs(n_input - n_dest) == std::abs(m_input - m_dest)) {
			return true;
		}
		return false;
	case ChessPiece::ROOK:
		if (n_input == n_dest || m_input == m_dest) {
			return true;
		}
		return false;
	case ChessPiece::KING:
		if (std::abs(n_input - n_dest) <= 1 && std::abs(m_input - m_dest) <= 1) {
			return true;
		}
		return false;
	case ChessPiece::QUEEN:
		if (n_input == n_dest || m_input == m_dest) {
			return true;
		}
		if (std::abs(n_input - n_dest) == std::abs(m_input - m_dest)) {
			return true;
		}
		return false;
	case ChessPiece::PAWN:
	{
		int8_t pos_dif_n = (piece.piece_team == ChessTeam::WHITE) ? 1 : -1;
		int8_t pos_dif_m = 1;
		if (array_ptr_[n_dest][m_dest].piece_type == ChessPiece::EMPTY) {
			pos_dif_m = 0;
		}
		if (n_input - n_dest == pos_dif_n && std::abs(m_input - m_dest) <= pos_dif_m) {
			return true;
		}
		return false;
	}
	}
}

// Collision with pieces in the path of movement
// Expected to run after CheckValidPieceSelected() and CheckLegalPieceMove()
bool Chess::CheckCollision(int n_input, int m_input, int n_dest, int m_dest) const {
	const BoardTile& piece_input = array_ptr_[n_input][m_input];
	const BoardTile& dest_tile = array_ptr_[n_dest][m_dest];
	if (dest_tile.piece_team == piece_input.piece_team) {
		return true;
	}
	switch (piece_input.piece_type) {
	default:                                      // default case includes (Knight, King), since no collison is possible
		return false;

	case ChessPiece::BISHOP:
	{
		int8_t increment_n = (n_input - n_dest > 0) ? -1 : 1;
		int8_t increment_m = (m_input - m_dest > 0) ? -1 : 1;
		int pos1 = n_input + increment_n;
		int pos2 = m_input + increment_m;
		while (pos1 != n_dest) {
			if (array_ptr_[pos1][pos2].piece_type != ChessPiece::EMPTY) {
				return true;
			}
			pos1 += increment_n;
			pos2 += increment_m;
		}
		return false;
	}
	case ChessPiece::ROOK:
	{
		int8_t increment_n = 0;
		int8_t increment_m = 0;
		if (n_input == n_dest) {
			increment_n = 0;
			increment_m = (m_input > m_dest) ? -1 : 1;
		}
		else {
			increment_m = 0;
			increment_n = (n_input > n_dest) ? -1 : 1;
		}
		int pos1 = n_input + increment_n;
		int pos2 = m_input + increment_m;
		while (pos1 != n_dest || pos2 != m_dest) {
			if (array_ptr_[pos1][pos2].piece_type != ChessPiece::EMPTY) {
				return true;
			}
			pos1 += increment_n;
			pos2 += increment_m;
		}
		return false;
	}
	case ChessPiece::QUEEN:
	{
		int8_t increment_n = (n_input > n_dest) ? -1 : 1;
		int8_t increment_m = (m_input > m_dest) ? -1 : 1;
		if (n_input == n_dest) {
			increment_n = 0;
		}
		if (m_input == m_dest) {
			increment_m = 0;
		}
		int pos1 = n_input + increment_n;
		int pos2 = m_input + increment_m;
		while (pos1 != n_dest || pos2 != m_dest) {
			if (array_ptr_[pos1][pos2].piece_type != ChessPiece::EMPTY) {
				return true;
			}
			pos1 += increment_n;
			pos2 += increment_m;
		}
		return false;
	}
	case ChessPiece::PAWN:
		if (m_input == m_dest) {
			return (array_ptr_[n_dest][m_dest].piece_type != ChessPiece::EMPTY);
		}
		return false;
	}
}
