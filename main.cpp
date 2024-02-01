#include "chess.h"
#include "board_visualizer.h"
#include "cpu_opponent.h"
#include "board_state_container.h"

#include <iostream>
#include <chrono>
#include <thread>

using namespace std;

void PrintDeque(const deque<pair<int, int>>& source) {
	for (const auto& element : source) {
		cout << '[' << element.first << ", " << element.second << ']';
	}
	cout << endl;
}

void PlayGameWithCPUS() {
	using namespace std::chrono_literals;
	Chess game1;
	RandomMovesPlayer white(game1, ChessTeam::WHITE);
	RandomMovesPlayer black(game1, ChessTeam::BLACK);
	BoardVisualizerFunc(game1);
	BoardStateContainer boards;
	for (int i = 1; i < 102; ++i) {
		cout << "Turn " << i++ << ": \n";
		white.AgrMovePiece();
		boards.RecordBoardState(game1);
		BoardVisualizerFunc(game1);
		std::this_thread::sleep_for(0s);
		cout << "Turn " << i << ": \n";
		black.MovePiece();
		boards.RecordBoardState(game1);
		BoardVisualizerFunc(game1);
		std::this_thread::sleep_for(0s);
	}
	cout << "\n\n\n\n";
	BoardVisualizerFunc(boards.GetBoardState(3));
	BoardVisualizerFunc(boards.GetBoardState(29));
}

int main() {
	PlayGameWithCPUS();
}