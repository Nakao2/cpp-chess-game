#include "chess_history.h"
#include "chess.h"

#include <tuple>

// Same as the parent class, but records board state every move
bool ChessWithHistory::MovePiece(std::pair<int, int> input_pos, std::pair<int, int> output_pos) {
	board_history_.RecordBoardState(*this);
	return Chess::MovePiece(input_pos, output_pos);
}

const BoardStateContainer& ChessWithHistory::GetHistory() const {
	return board_history_;
}

Chess ChessWithHistory::GetBoardState(int turn_num) const {
	return board_history_.GetBoardState(turn_num);
}
