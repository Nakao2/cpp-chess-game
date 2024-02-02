#include "board_state_container.h"
#include "chess.h"
#include <deque>
#include <stdexcept>

using namespace std;

void BoardStateContainer::RecordBoardState(const Chess& source) {
	boards_.emplace_back(source);
}

Chess BoardStateContainer::GetBoardState(int turn_num) const {
	if (turn_num < 0 || turn_num > boards_.size()) {
		throw std::invalid_argument("Invalid turn number"s);
	}
	return boards_[turn_num - 1];
}

void BoardStateContainer::Clear() {
	boards_.clear();
}

size_t BoardStateContainer::GetSize() const {
	return boards_.size();
}
