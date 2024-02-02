#pragma once

#include "chess.h"
#include <deque>

// Stores board states (objects of type 'Chess')
// Is not tied to any particular game
class BoardStateContainer {
public:

	void RecordBoardState(const Chess& source);

	Chess GetBoardState(int turn_num) const;

	void Clear();

	size_t GetSize() const;

private:
	std::deque<Chess> boards_;
};