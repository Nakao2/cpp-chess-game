#pragma once

#include "chess.h"

#include <tuple>

uint8_t GivePieceValue(ChessPiece piece) {
	switch (piece) {
	default:
		return 0i8;
	case ChessPiece::PAWN:
		return 1i8;
	case ChessPiece::KNIGHT:
		return 3i8;
	case ChessPiece::BISHOP:
		return 3i8;
	case ChessPiece::ROOK:
		return 5i8;
	case ChessPiece::QUEEN:
		return 9i8;
	}	
}

uint32_t GiveBoardValue(const Chess& board, ChessTeam team) {
	uint32_t counter = 0;
	std::pair<int, int> dims = board.GetDimensions();
	for (int n = 0; n < dims.first; ++n) {
		for (int m = 0; m < dims.second; ++m) {
			BoardTile tile = board.LookUp(n, m);
			if (tile.piece_team == team) {
				counter += GivePieceValue(tile.piece_type);
			}
		}
	}
	return counter;
}

/*class ValueCalculator {
public:

	ValueCalculator() = default;

	ValueCalculator(const Chess& board) {
		std::pair<int, int> dims = board.GetDimensions();
		for (int n = 0; n < dims.first; ++n) {
			for (int m = 0; m < dims.second; ++m) {
				BoardTile tile = board.LookUp(n, m);
				if (tile.piece_team == ChessTeam::WHITE) {
					white_ += GivePieceValue(tile.piece_type);
				}
				if (tile.piece_team == ChessTeam::BLACK) {
					black_ += GivePieceValue(tile.piece_type);
				}
			}
		}
	}

	uint32_t GetTotalValue(const ChessTeam team) const {
		if (team == ChessTeam::WHITE) {
			return white_;
		}
		else {
			return black_;
		}
	}

	// Do not give empty tiles to this method
	void Remove(const BoardTile& tile) {
		if (tile.piece_team == ChessTeam::WHITE) {
			white_ -= GivePieceValue(tile.piece_type);
		}
		else {
			black_ -= GivePieceValue(tile.piece_type);
		}
	}
	void Add(const BoardTile& tile) {
		if (tile.piece_team == ChessTeam::WHITE) {
			white_ += GivePieceValue(tile.piece_type);
		}
		else if (tile.piece_team == ChessTeam::BLACK) {
			black_ += GivePieceValue(tile.piece_type);
		}
	}

private:
	uint32_t white_ = 0;
	uint32_t black_ = 0;

};
*/