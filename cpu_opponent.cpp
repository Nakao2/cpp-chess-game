#include "cpu_opponent.h"

#include <tuple>
#include <random>
#include <deque>
#include <iostream>

constexpr uint16_t MAX_NUM_OF_ATTEMPTS = 1024;

using namespace std;

// Creates local array of tile coordinates that contain pieces of a specified team
// Class will not recognize newly created pieces of its team on the board
RandomMovesPlayer::RandomMovesPlayer(Chess& board, const ChessTeam& team) {
	team_ = team;
	pair<int, int> dim = board.GetDimensions();
	deque<pair<int, int>> pieces_pos;
	for (int n = 0; n < dim.first; ++n) {
		for (int m = 0; m < dim.second; ++m) {
			if (board.LookUp(n, m).piece_team == team) {
				pieces_pos.push_back({ n, m });
			}
		}
	}
	array_ptr_ = new pair<int, int>[pieces_pos.size()];
	size_ = pieces_pos.size();
	max_size_ = pieces_pos.size();
	for (size_t i = 0; i < size_; ++i) {
		array_ptr_[i] = pieces_pos[i];
	}
	board_ = &board;
}

// Moves random piece to a random tile in accordance with chess rules
// May or may not capture enemy pieces
void RandomMovesPlayer::MovePiece() {
	this->RemoveCapturedPieces();
	if (size_ == 0) {
		cout << "No pieces to move"s << endl;
		return;
	}

	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> distr1(0, size_ - 1);

	pair<int, int> rand_input_pos;
	int input_rand_num = 0;
	deque<pair<int, int>> output_pos;
	uint16_t attempts = 0;

	while (output_pos.empty() && attempts < MAX_NUM_OF_ATTEMPTS) {
		input_rand_num = distr1(gen);
		rand_input_pos = array_ptr_[input_rand_num];
		output_pos = board_->GetPossibleDestTiles(rand_input_pos.first, rand_input_pos.second);
		++attempts;
	}

	if (!output_pos.empty()) {
		std::uniform_int_distribution<> distr2(0, output_pos.size() - 1);
		pair<int, int> rand_output_pos = output_pos[distr2(gen)];
		cout << "Attempt to move piece at [" << rand_input_pos.first << ", " << rand_input_pos.second <<
			"] to a position [" << rand_output_pos.first << ", " << rand_output_pos.second << ']' << '\n' << endl;
		board_->MovePiece(rand_input_pos, rand_output_pos);
		array_ptr_[input_rand_num] = rand_output_pos;
	}
}

// If available, makes a move that captures enemy piece
// If not, makes a random move that is guaranteed to not capture a piece
void RandomMovesPlayer::AgrMovePiece() {
	this->RemoveCapturedPieces();
	if (size_ == 0) {
		cout << "No pieces to move"s << endl;
		return;
	}

	pair<int, int>** pieces_pos_ptrs = new pair<int, int>* [size_];
	int pieces_left_to_check = size_;
	bool capture_success = false;
	for (int i = 0; i < size_; ++i) {
		pieces_pos_ptrs[i] = &array_ptr_[i];
	}
	std::random_device rd;
	std::mt19937 gen(rd());
	for (int pieces_left_to_check = size_; pieces_left_to_check > 0; --pieces_left_to_check) { // Go through each available piece once
		std::uniform_int_distribution<> distr1(0, pieces_left_to_check - 1);
		int rand_num = distr1(gen);
		pair<int, int> rand_input_pos = *pieces_pos_ptrs[rand_num];
		pair<int, int> rand_output_pos;

		deque<pair<int, int>> moves = std::move(board_->GetPossibleDestTiles(rand_input_pos.first, rand_input_pos.second));
		deque<pair<int, int>> capture_moves;
		for (pair<int, int>& pos : moves) {  // Checks if any move for a random piece can capture
			if (board_->LookUp(pos.first, pos.second).piece_type != ChessPiece::EMPTY) {
				capture_moves.push_back(std::move(pos));
			}
		}

		if (!capture_moves.empty()) {
			std::uniform_int_distribution<> distr2(0, capture_moves.size() - 1);
			int rand_num = distr2(gen);
			rand_output_pos = capture_moves[rand_num];           // Randomly select what piece to capture
			board_->MovePiece(rand_input_pos, rand_output_pos);
			*pieces_pos_ptrs[rand_num] = rand_output_pos;        // Piece moved, modify our positions database
			capture_success = true;
			pieces_left_to_check = 1;                            // This ends the cycle
			std::cout << "Piece at [" << rand_input_pos.first << ", " << rand_input_pos.second <<
				"] captured piece at [" << rand_output_pos.first << ", " << rand_output_pos.second << "]\n" << endl;
		}
		pieces_pos_ptrs[rand_num] = pieces_pos_ptrs[pieces_left_to_check - 1]; // Piece checked, don't need to do it again
	}

	delete[] pieces_pos_ptrs;
	if (!capture_success) {
		this->MovePiece();
	}
}

// Finds and removes pieces from database that no longer exist on the board
void RandomMovesPlayer::RemoveCapturedPieces() {
	for (size_t i = 0; i < size_; ++i) {
		if (board_->LookUp(array_ptr_[i].first, array_ptr_[i].second).piece_team != team_) {
			array_ptr_[i] = array_ptr_[size_ - 1];               // Removal at O(1), but sequence of elements is broken
			--size_;
			break;                // Removes unecessary checks, if the maximum number of captured pieces per turn is 1
		}
	}
}
