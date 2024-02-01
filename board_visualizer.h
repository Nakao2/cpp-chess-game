#pragma once

#include "chess.h"
#include <iostream>
#include <tuple>

char GetCharForPiece(const ChessPiece& piece) {
	switch (piece) {
	default:
		return '#';
	case ChessPiece::PAWN:
		return 'P';
	case ChessPiece::ROOK:
		return 'R';
	case ChessPiece::BISHOP:
		return 'B';
	case ChessPiece::KNIGHT:
		return 'N';
	case ChessPiece::QUEEN:
		return 'Q';
	case ChessPiece::KING:
		return 'K';
	}
}

void BoardVisualizerFunc(const Chess& board) {
	using namespace std::string_literals;
	if (board.WhoseMove() == ChessTeam::WHITE) {
		std::cout << "Whites move"s;
	}
	else {
		std::cout << "Blacks move"s;
	}
	std::cout << '\n';
	const std::pair<int, int> dim = board.GetDimensions();
	std::cout << ' ' << ' ' << ' ' << ' ';
	for (int columns = 0; columns < dim.second; ++columns) {
		std::cout << columns << ' ' << ' ' << ' ';
	}
	std::cout << '\n';
	for (int rows = 0; rows < dim.first; ++rows) {
		std::cout << rows << ' ' << ' ' << ' ';
		for (int columns = 0; columns < dim.second; ++columns) {
			const BoardTile& tile = board.LookUp(rows, columns);
			char team = ' ';
			if (tile.piece_team == ChessTeam::BLACK) {
				team = 'b';
			}
			if (tile.piece_team == ChessTeam::WHITE) {
				team = 'w';
			}
			std::cout << GetCharForPiece(tile.piece_type) << team << ' ' << ' ';
		}
		std::cout << '\n' << '\n';
	}
	std::cout << '\n' << '\n';
}