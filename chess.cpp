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
		array_ptr_[row][0] = { ChessPiece::ROOK, team, false };
		array_ptr_[row][1] = { ChessPiece::KNIGHT, team, false };
		array_ptr_[row][2] = { ChessPiece::BISHOP, team, false };
		array_ptr_[row][3] = { ChessPiece::QUEEN, team, false };
		array_ptr_[row][4] = { ChessPiece::KING, team, false };
		array_ptr_[row][5] = { ChessPiece::BISHOP, team, false };
		array_ptr_[row][6] = { ChessPiece::KNIGHT, team, false };
		array_ptr_[row][7] = { ChessPiece::ROOK, team, false };
	}
	for (int column = 0; column < columns_; ++column) {
		array_ptr_[1][column] = { ChessPiece::PAWN, ChessTeam::BLACK, false };
	}
	for (int column = 0; column < columns_; ++column) {
		array_ptr_[6][column] = { ChessPiece::PAWN, ChessTeam::WHITE, false };
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
	this->en_passant_ = source.en_passant_;
	this->pawn_promotion_ = source.pawn_promotion_;
	this->is_whites_move_ = source.is_whites_move_;
}

Chess::Chess(Chess&& source) {
	std::swap(array_ptr_, source.array_ptr_);
	std::swap(rows_, source.rows_);
	std::swap(columns_, source.columns_);
	is_whites_move_ = source.is_whites_move_;
	en_passant_ = source.en_passant_;
	pawn_promotion_ = source.pawn_promotion_;
}

// Copies board state. Previous state of *this is destroyed
Chess& Chess::operator=(const Chess& source) {
	if (&source == this) {
		return *this;
	}
	Chess copy(source);
	// Swap necessary for copy's destructor
	std::swap(this->array_ptr_, copy.array_ptr_);
	std::swap(this->rows_, copy.rows_);
	std::swap(this->columns_, copy.columns_);
	this->en_passant_ = copy.en_passant_;
	this->pawn_promotion_ = copy.pawn_promotion_;
	this->is_whites_move_ = copy.is_whites_move_;
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
	FillBoardWith({ ChessPiece::PAWN, ChessTeam::WHITE, false });
}

void Chess::EmptyBoard() {
	FillBoardWith({ ChessPiece::EMPTY, ChessTeam::NEUTRAL, false });
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
			if (CheckLegalPieceMove(n_input, m_input, n, m)) {
				// Castling
				if (array_ptr_[n_input][m_input].piece_type == ChessPiece::KING && std::abs(m_input - m) == 2) {
					if(CastlingCheckRequirements(n_input, m_input, n, m)){
						output.push_back({ n, m });
					}
				}
				// En passant
				else if (array_ptr_[n_input][m_input].piece_type == ChessPiece::PAWN &&
					en_passant_.first && n == en_passant_.second.first && m == en_passant_.second.second) {
					BoardTile enemy_pawn = array_ptr_[n_input][m];
					if (enemy_pawn.piece_team == ChessTeam::NEUTRAL) {
						continue;
					}
					array_ptr_[n][m] = array_ptr_[n_input][m_input];
					array_ptr_[n_input][m_input] = { ChessPiece::EMPTY, ChessTeam::NEUTRAL, false };
					array_ptr_[n_input][m] = { ChessPiece::EMPTY, ChessTeam::NEUTRAL, false };
					if (!IsCheck(array_ptr_[n][m].piece_team)) {
						output.push_back({ n, m });
					}
					array_ptr_[n_input][m_input] = array_ptr_[n][m];
					array_ptr_[n][m] = { ChessPiece::EMPTY, ChessTeam::NEUTRAL, false };
					array_ptr_[n_input][m] = enemy_pawn;
				}
				// All else
				else if (!CheckCollision(n_input, m_input, n, m)){
					BoardTile dest_tile = array_ptr_[n][m];
					array_ptr_[n][m] = array_ptr_[n_input][m_input];
					array_ptr_[n_input][m_input] = { ChessPiece::EMPTY, ChessTeam::NEUTRAL, false };
					if (!IsCheck(array_ptr_[n][m].piece_team)) {
						output.push_back({ n, m });
					}
					array_ptr_[n_input][m_input] = array_ptr_[n][m];
					array_ptr_[n][m] = dest_tile;
				}
			}
		}
	}
	return output;
}

// Does all the necessary checks, moves a piece and returns 'true'
// Or does nothing and returns 'false' if the move is illegal
bool Chess::MovePiece(pair<int, int> input_pos, pair<int, int> dest_pos) {
	if (CheckValidPieceSelected(input_pos.first, input_pos.second) && CheckCorrectTurnSequence(input_pos.first, input_pos.second) &&
		CheckLegalPieceMove(input_pos.first, input_pos.second, dest_pos.first, dest_pos.second)) {

		if (array_ptr_[input_pos.first][input_pos.second].piece_type == ChessPiece::KING &&  // Castling shenanigans
			std::abs(input_pos.second - dest_pos.second) == 2) {
			if (CastlingCheckRequirements(input_pos.first, input_pos.second, dest_pos.first, dest_pos.second)) {
				int increment_m = (input_pos.second > dest_pos.second) ? 1 : -1;
				BoardTile Rook;
				int m_pos = input_pos.second;
				while (Rook.piece_type != ChessPiece::ROOK) {
					m_pos -= increment_m;
					Rook = array_ptr_[input_pos.first][m_pos];
				}
				array_ptr_[input_pos.first][m_pos] = { ChessPiece::EMPTY, ChessTeam::NEUTRAL, false };
				Rook.has_moved = true;
				array_ptr_[input_pos.first][dest_pos.second + increment_m] = Rook;
				array_ptr_[dest_pos.first][dest_pos.second] = array_ptr_[input_pos.first][input_pos.second];
				array_ptr_[dest_pos.first][dest_pos.second].has_moved = true;
				array_ptr_[input_pos.first][input_pos.second] = { ChessPiece::EMPTY, ChessTeam::NEUTRAL, false };
				is_whites_move_ = (is_whites_move_) ? 0 : 1;
				en_passant_.first = false;
				return true;
			}
			return false;
		}

		if (array_ptr_[input_pos.first][input_pos.second].piece_type == ChessPiece::PAWN && // En passant logic
			en_passant_.first &&
			dest_pos.first == en_passant_.second.first && dest_pos.second == en_passant_.second.second) {

			BoardTile enemy_pawn = array_ptr_[input_pos.first][dest_pos.second];
			if (enemy_pawn.piece_team == ChessTeam::NEUTRAL) { // Pointless with classic turn sequence
				return false;
			}
			ForceMove(input_pos.first, input_pos.second, dest_pos.first, dest_pos.second);
			array_ptr_[input_pos.first][dest_pos.second] = { ChessPiece::EMPTY, ChessTeam::NEUTRAL, false };

			if (IsCheck(WhoseMove())) {
				array_ptr_[input_pos.first][dest_pos.second] = enemy_pawn;
				array_ptr_[input_pos.first][input_pos.second] = array_ptr_[dest_pos.first][dest_pos.second];
				array_ptr_[dest_pos.first][dest_pos.second] = { ChessPiece::EMPTY, ChessTeam::NEUTRAL, false };
				return false;
			}

			en_passant_.first = false;
			array_ptr_[dest_pos.first][dest_pos.second].has_moved = true;
			is_whites_move_ = (is_whites_move_) ? 0 : 1;
			return true;
		}


		if (!CheckCollision(input_pos.first, input_pos.second, dest_pos.first, dest_pos.second)){
			BoardTile dest_tile = array_ptr_[dest_pos.first][dest_pos.second];
			ForceMove(input_pos.first, input_pos.second, dest_pos.first, dest_pos.second);

			if (IsCheck(WhoseMove())) { // A move that makes it possible for an opponent to capture king is illegal
				array_ptr_[input_pos.first][input_pos.second] = array_ptr_[dest_pos.first][dest_pos.second];
				array_ptr_[dest_pos.first][dest_pos.second] = dest_tile;
				return false;
			}

			en_passant_.first = false;
			dest_tile = array_ptr_[dest_pos.first][dest_pos.second];
			if (dest_tile.piece_type == ChessPiece::PAWN) {
				// Record en passant to memory
				if (std::abs(dest_pos.first - input_pos.first) == 2) {
					int8_t increment_n = dest_tile.piece_team == ChessTeam::WHITE ? 1 : -1;
					en_passant_.first = true;
					en_passant_.second = { dest_pos.first + increment_n, dest_pos.second };
				}
				// If pawn reached the end of the board
				if (dest_pos.first == 0 || dest_pos.first == rows_ - 1) {
					pawn_promotion_.first = true;
					pawn_promotion_.second = { dest_pos.first, dest_pos.second };
				}
			}
			array_ptr_[dest_pos.first][dest_pos.second].has_moved = true;
			is_whites_move_ = (is_whites_move_) ? 0 : 1;
			return true;
		}
	}
	return false;
}


pair<int, int> Chess::GetDimensions() const {
	return { rows_, columns_ };
}

ChessTeam Chess::WhoseMove() const {
	return (is_whites_move_) ? ChessTeam::WHITE : ChessTeam::BLACK;
}

[[nodiscard]] bool Chess::PawnPromotion() const {
	return pawn_promotion_.first;
}

// Requires a wrapper to work properly
// Otherwise turn sequence and team ownership are ignored
void Chess::PawnPromotion(ChessPiece piece) {
	array_ptr_[pawn_promotion_.second.first][pawn_promotion_.second.second].piece_type = piece;
	pawn_promotion_.first = false;
}

bool Chess::CheckOutOfBounds(int row, int column) const {
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
		if (n_input == n_dest && std::abs(m_input - m_dest) == 2) { // Castling
			return true;
		}
		if (std::abs(n_input - n_dest) <= 1 && std::abs(m_input - m_dest) <= 1) { // Regular king move
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
		int8_t min_dif_n;
		int8_t max_dif_n;
		if (piece.piece_team == ChessTeam::WHITE) {
			min_dif_n = 1;
			max_dif_n = piece.has_moved ? 1 : 2;
		}
		else {
			min_dif_n = -1;
			max_dif_n = piece.has_moved ? -1 : -2;
		}
		int8_t pos_dif_m = 0;
		if (array_ptr_[n_dest][m_dest].piece_type != ChessPiece::EMPTY || 
			(en_passant_.first && n_dest == en_passant_.second.first && m_dest == en_passant_.second.second)) {
			pos_dif_m = 1;
			max_dif_n = (max_dif_n > 0) ? 1 : -1;
		}
		if ((n_input - n_dest == min_dif_n || n_input - n_dest == max_dif_n) && std::abs(m_input - m_dest) <= pos_dif_m) {
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
			int8_t increment_n = (piece_input.piece_team == ChessTeam::WHITE) ? -1 : 1;
			int pos1 = n_input;
			while (pos1 != n_dest) {
				pos1 += increment_n;
				if (array_ptr_[pos1][m_dest].piece_type != ChessPiece::EMPTY) {
					return true;
				}
			}
		}
		return false;
	}
}

// Finds if a king of a specified team is checked
bool Chess::IsCheck(ChessTeam team) const {
	std::pair<int, int> king_pos;
	bool king_found = false;
	for (int row = 0; row != rows_ && !king_found; ++row) { // Find king of specified team on the board
		for (int column = 0; column != columns_ && !king_found; ++column) {
			if (array_ptr_[row][column].piece_team == team &&
				array_ptr_[row][column].piece_type == ChessPiece::KING) {

				king_pos = { row, column };
				king_found = true;
			}
		}
	}
	team = (team == ChessTeam::WHITE) ? ChessTeam::BLACK : ChessTeam::WHITE;
	for (int row = 0; row != rows_; ++row) { // Check if any opposing team's piece can capture the king
		for (int column = 0; column != columns_; ++column) {
			if (array_ptr_[row][column].piece_team == team &&
				CheckLegalPieceMove(row, column, king_pos.first, king_pos.second) && 
				!CheckCollision(row, column, king_pos.first, king_pos.second)) {

				if (array_ptr_[row][column].piece_type == ChessPiece::KING && std::abs(column - king_pos.second) == 2) {
					continue;
				}
				return true;
			}
		}
	}
	return false;
}

// Run after CheckLegalPieceMove() for castling
// King must be at input position
bool Chess::CastlingCheckRequirements(int n_in, int m_in, int n_dest, int m_dest) const {
	if (array_ptr_[n_in][m_in].has_moved || IsCheck(array_ptr_[n_in][m_in].piece_team)) {
		return false;
	}
	int increment_m = (m_in > m_dest) ? -1 : 1;
	BoardTile rook;
	int pos = m_in + increment_m;
	for (; !CheckOutOfBounds(n_in, pos); pos += increment_m) {
		if (array_ptr_[n_in][pos].piece_type == ChessPiece::ROOK) {
			rook = array_ptr_[n_in][pos];
			break;
		}
		if (array_ptr_[n_in][pos].piece_type != ChessPiece::EMPTY) {
			break;
		}
	}
	if (rook.piece_type != ChessPiece::ROOK || rook.has_moved) {
		return false;
	}

	array_ptr_[n_in][pos] = { ChessPiece::EMPTY, ChessTeam::NEUTRAL, false };
	if (array_ptr_[n_dest][m_dest].piece_type != ChessPiece::EMPTY) {
		array_ptr_[n_in][pos] = rook;
		return false;
	}
	BoardTile king = array_ptr_[n_in][m_in]; // Puts king into tiles in path of movement and sees if there is a check
	array_ptr_[n_in][m_in] = { ChessPiece::EMPTY, ChessTeam::NEUTRAL, false };
	array_ptr_[n_in][m_in + increment_m] = king;
	if (IsCheck(king.piece_team)) { // If check, return the board to the previous state
		array_ptr_[n_in][m_in + increment_m] = { ChessPiece::EMPTY, ChessTeam::NEUTRAL, false };
		array_ptr_[n_in][pos] = rook;
		array_ptr_[n_in][m_in] = king;
		return false;
	}
	array_ptr_[n_in][m_in + increment_m] = { ChessPiece::EMPTY, ChessTeam::NEUTRAL, false };
	array_ptr_[n_in][m_in + 2 * increment_m] = king;
	bool output = true;
	if (IsCheck(king.piece_team)) {
		output = false;
	}
	array_ptr_[n_in][m_in + 2 * increment_m] = { ChessPiece::EMPTY, ChessTeam::NEUTRAL, false };
	array_ptr_[n_in][pos] = rook;
	array_ptr_[n_in][m_in] = king;
	return output;
}

