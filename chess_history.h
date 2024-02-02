#pragma once

#include "chess.h"
#include "board_state_container.h"

#include <tuple>

// Derived class of 'Chess' with board recording feature
class ChessWithHistory :public Chess {
public:

	ChessWithHistory(int n, int m) : Chess(n, m) {}

	ChessWithHistory() : Chess() {}

	~ChessWithHistory() override = default;

	// Same as the parent class, but records board state every move
	bool MovePiece(std::pair<int, int> input_pos, std::pair<int, int> output_pos) override;

	const BoardStateContainer& GetHistory() const;

	Chess GetBoardState(int turn_num) const;

private:
	BoardStateContainer board_history_;
};