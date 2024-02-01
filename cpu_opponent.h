#pragma once

#include "chess.h"
#include <tuple>


class RandomMovesPlayer {
public:

	// Creates local array of tile coordinates that contain pieces of a specified team
	// Class will not recognize newly created pieces of its team on the board
	RandomMovesPlayer(Chess& board, const ChessTeam& team = ChessTeam::BLACK);

	RandomMovesPlayer() = delete;

	~RandomMovesPlayer() {
		CleanUp();
	}

	// Moves random piece to a random tile in accordance with chess rules
	// May or may not capture enemy pieces
	void MovePiece();

	// If available, makes a move that captures enemy piece
	// If not, makes a random move that is guaranteed to not capture a piece
	void AgrMovePiece();

private:
	Chess* board_ = nullptr;
	std::pair<int, int>* array_ptr_ = nullptr;
	size_t size_ = 0;
	size_t max_size_ = 0;
	ChessTeam team_;

	// Finds and removes pieces from database that no longer exist on the board
	void RemoveCapturedPieces();

	void CleanUp() {
		delete[] array_ptr_;
	}
};
