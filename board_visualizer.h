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
	for (int m = 0; m < dim.second; ++m) {
		cout << m << ' ' << ' ' << ' ';
	}
	cout << '\n';
	for (int n = 0; n < dim.first; ++n) {
		cout << n << ' ' << ' ' << ' ';
		for (int m = 0; m < dim.second; ++m) {
			const BoardTile& tile = board.LookUp(n, m);
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