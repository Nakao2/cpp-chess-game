#pragma once

#include "chess.h"
#include <iostream>

using namespace std;

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
		return 'K';
	case ChessPiece::QUEEN:
		return 'Q';
	case ChessPiece::KING:
		return '$';
	}
}

void BoardVisualizerFunc(const Chess& board) {
	const pair<int, int> dim = board.GetDimensions();
	cout << ' ' << ' ' << ' ' << ' ';
	for (int columns = 0; columns < dim.second; ++columns) {
		cout << columns << ' ' << ' ' << ' ';
	}
	cout << '\n';
	for (int rows = 0; rows < dim.first; ++rows) {
		cout << rows << ' ' << ' ' << ' ';
		for (int columns = 0; columns < dim.second; ++columns) {
			const BoardTile& tile = board.LookUp(rows, columns);
			char team = ' ';
			if (tile.piece_team == ChessTeam::BLACK) {
				team = 'b';
			}
			if (tile.piece_team == ChessTeam::WHITE) {
				team = 'w';
			}
			cout << GetCharForPiece(tile.piece_type) << team << ' ' << ' ';
		}
		cout << '\n' << '\n';
	}
	cout << '\n' << '\n';
}