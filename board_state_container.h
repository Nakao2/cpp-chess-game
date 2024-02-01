#pragma once

#include "chess.h"
#include <deque>

class BoardStateContainer {
public:

	void RecordBoardState(const Chess& source);

	Chess GetBoardState(int turn_num) const;

	void Clear();

private:
	deque<Chess> boards_;
};