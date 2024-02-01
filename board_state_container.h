#pragma once

#include "chess.h"
#include <deque>

// Stores board states (objects of type 'Chess')
// Is not tied to any particular game
// Meaning, can store boards of various sizes at the same time
class BoardStateContainer {
public:

	void RecordBoardState(const Chess& source);

	Chess GetBoardState(int turn_num) const;

	void Clear();

private:
	std::deque<Chess> boards_;
};