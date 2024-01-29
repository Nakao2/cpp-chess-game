#pragma once

#include <vector>
#include <iostream>
#include <cassert>
#include <new>
#include <deque>

using namespace std;

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
};

class Chess {
public:

	Chess(int x, int y) {             // Board has matrix-like dimensions of (n x m), where an element of 1x1 board has coordinates (0, 0)
		rows = x;
		columns = y;
		try {
			BoardTile** row_ptr = new BoardTile * [x];
			array_ptr = row_ptr;
			for (int i = 0; i < x; ++i) {
				BoardTile* column_ptr = new BoardTile[y];
				row_ptr[i] = column_ptr;
			}
		}
		catch (const bad_alloc&) {
			CleanUp();
			throw;
		}
	}
	Chess() : Chess(8,8) {
		for (int i = 0; i < rows; i += 7) {                                  // Classic game of chess piece setup
			ChessTeam team = (i == 0) ? ChessTeam::BLACK : ChessTeam::WHITE;
			array_ptr[i][0] = { ChessPiece::ROOK, team };
			array_ptr[i][1] = { ChessPiece::KNIGHT, team };
			array_ptr[i][2] = { ChessPiece::BISHOP, team };
			array_ptr[i][3] = { ChessPiece::QUEEN, team };
			array_ptr[i][4] = { ChessPiece::KING, team };
			array_ptr[i][5] = { ChessPiece::BISHOP, team };
			array_ptr[i][6] = { ChessPiece::KNIGHT, team };
			array_ptr[i][7] = { ChessPiece::ROOK, team };
		}
		for (int i = 0; i < columns; ++i) {
			array_ptr[1][i] = { ChessPiece::PAWN, ChessTeam::BLACK };
		}
		for (int i = 0; i < columns; ++i) {
			array_ptr[6][i] = { ChessPiece::PAWN, ChessTeam::WHITE };
		}
	}
	Chess(const Chess& copy) = delete;

	~Chess() {
		CleanUp();
	}

	BoardTile LookUp(int row, int column) const {
		if (!CheckOutOfBounds(row, column)) {
			return array_ptr[row][column];
		}
		return {};
	}

	void FillBoardWith(const BoardTile& piece) {
		for (int i = 0; i < rows; ++i) {
			for (int k = 0; k < columns; ++k) {
				array_ptr[i][k] = piece;
			}
		}
	}
	void FillBoardWithPawns() {
		FillBoardWith({ ChessPiece::PAWN, ChessTeam::WHITE });
	}
	void EmptyBoard() {
		FillBoardWith({ ChessPiece::EMPTY, ChessTeam::NEUTRAL });
	}

	void PutPieceInPosition(const BoardTile& piece, int row, int column) {
		if (!CheckOutOfBounds(row, column)) {
			array_ptr[row][column] = piece;
		}
	}

	// Gives all possible destination tiles on the board for a selected piece
	deque<pair<int, int>> GetPossibleDestTiles(int n_input, int m_input) const {  // Brute force. Inefficient for large boards
		deque<pair<int, int>> output;
		if (!CheckValidPieceSelected(n_input, m_input)) {
			return output;
		}
		for (int n = 0; n < rows; ++n) {
			for (int m = 0; m < columns; ++m) {
				if (CheckLegalPieceMove(n_input, m_input, n, m) && !CheckCollision(n_input, m_input, n, m)) {
					output.push_back({n, m});
				}
			}
		}
		return output;
	}

	// Does all the necessary checks and moves a piece
	// Or does nothing if the move is illegal
	void MovePiece(int n_input, int m_input, int n_dest, int m_dest) {         // Convert to bool to indicate successful move?
		if (CheckValidPieceSelected(n_input, m_input) && CheckCorrectTurnSequence(n_input, m_input) && 
			CheckLegalPieceMove(n_input, m_input, n_dest, m_dest) && !CheckCollision(n_input, m_input, n_dest, m_dest)) {

			array_ptr[n_dest][m_dest] = array_ptr[n_input][m_input];
			array_ptr[n_input][m_input] = { ChessPiece::EMPTY, ChessTeam::NEUTRAL };
			is_whites_move = (is_whites_move) ? 0 : 1;
		}
		else {
			cout << "Illegal move"s << endl;
		}
	}

	pair<int, int> GetDimensions() const {
		return { rows, columns };
	}
	ChessTeam WhoseMove() const {
		return (is_whites_move) ? ChessTeam::WHITE : ChessTeam::BLACK;
	}

private:
	BoardTile** array_ptr;
	int rows, columns;
	bool is_whites_move = true;

	bool CheckOutOfBounds(int row, int column) const {
		if ((row >= rows) || (row < 0)) {
			return true;
		}
		if ((column >= columns) || (column < 0)) {
			return true;
		}
		return false;
	}

	// Can't move OutOfBounds or EMPTY tile
	bool CheckValidPieceSelected(int n_input, int m_input) const {
		if (!CheckOutOfBounds(n_input, m_input)) {
			return (array_ptr[n_input][m_input].piece_type != ChessPiece::EMPTY);
		}
		cout << "Invalid piece selected" << endl;
		return false;
	}

	// Can't move black pieces at whites turn
	bool CheckCorrectTurnSequence(int n_input, int m_input) const {
		if (array_ptr[n_input][m_input].piece_team == ChessTeam::WHITE) {
			return is_whites_move;
		}
		else {
			return !is_whites_move;
		}
	}

	
	// Checks piece movement according to chess rules
	// Also does OutOfBounds check for destination tile
	// Expected to run after CheckValidPieceSelected()	
	bool CheckLegalPieceMove(int n_input, int m_input, int n_destination, int m_destination) const {
		if (n_input == n_destination && m_input == m_destination) {
			return false;
		}
		if (CheckOutOfBounds(n_destination, m_destination)) {
			return false;
		}
		const BoardTile& piece = array_ptr[n_input][m_input];
		switch (piece.piece_type) {
		default:
			std::cout << "No chess pieces at that position"s << endl;
			return false;
		case ChessPiece::KNIGHT:
		{
			int n_abs_dif = std::abs(n_input - n_destination);
			if (n_abs_dif == 2 || n_abs_dif == 1) {
				if (n_abs_dif + std::abs(m_input - m_destination) == 3) {
					return true;
				}
			}
			return false;
		}
		case ChessPiece::BISHOP:
			if (std::abs(n_input - n_destination) == std::abs(m_input - m_destination)) {
				return true;
			}
			return false;
		case ChessPiece::ROOK:
			if (n_input == n_destination || m_input == m_destination) {
				return true;
			}
			return false;
		case ChessPiece::KING:
			if (std::abs(n_input - n_destination) <= 1 && std::abs(m_input - m_destination) <= 1) {
				return true;
			}
			return false;
		case ChessPiece::QUEEN:
			if (n_input == n_destination || m_input == m_destination) {
				return true;
			}
			if (std::abs(n_input - n_destination) == std::abs(m_input - m_destination)) {
				return true;
			}
			return false;
		case ChessPiece::PAWN:
		{
			int8_t pos_dif_n = (piece.piece_team == ChessTeam::WHITE) ? 1 : -1;
			int8_t pos_dif_m = 1;
			if (array_ptr[n_destination][m_destination].piece_type == ChessPiece::EMPTY) {
				pos_dif_m = 0;
			}
			if (n_input - n_destination == pos_dif_n && std::abs(m_input - m_destination) <= pos_dif_m) {
				return true;
			}
			return false;
		}
		}
	}

	// Collision with pieces in the path of movement
    // Expected to run after CheckValidPieceSelected() and CheckLegalPieceMove()
	bool CheckCollision(int n_input, int m_input, int n_dest, int m_dest) const {
		const BoardTile& piece_input = array_ptr[n_input][m_input];
		const BoardTile& dest_tile = array_ptr[n_dest][m_dest];
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
				if (array_ptr[pos1][pos2].piece_type != ChessPiece::EMPTY) {
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
				if (array_ptr[pos1][pos2].piece_type != ChessPiece::EMPTY) {
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
				if (array_ptr[pos1][pos2].piece_type != ChessPiece::EMPTY) {
					return true;
				}
				pos1 += increment_n;
				pos2 += increment_m;
			}
			return false;
		}
		case ChessPiece::PAWN:
			if (m_input == m_dest) {
				return (array_ptr[n_dest][m_dest].piece_type != ChessPiece::EMPTY);
			}
			return false;
		}
	}

	void CleanUp() {
		for (int i = 0; i < rows; ++i) {
			delete[] array_ptr[i];
		}
		array_ptr = nullptr;
	}

};