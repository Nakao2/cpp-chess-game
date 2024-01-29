#include "chess.h"
#include "board_visualizer.h"
#include "cpu_opponent.h"

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
	for (int i = 1; i < 102; ++i) {
		cout << "Turn " << i++ << ": \n";
		white.AgrMovePiece();
		BoardVisualizerFunc(game1);
		std::this_thread::sleep_for(0s);
		cout << "Turn " << i << ": \n";
		black.MovePiece();
		BoardVisualizerFunc(game1);
		std::this_thread::sleep_for(0s);
	}
	std::cin.get();
}

int main() {
	Chess testgame;
	testgame.EmptyBoard();
	testgame.PutPieceInPosition({ ChessPiece::QUEEN, ChessTeam::WHITE }, 0, 3);
	testgame.PutPieceInPosition({ ChessPiece::ROOK, ChessTeam::BLACK }, 0, 0);
	BoardVisualizerFunc(testgame);
	PrintDeque(testgame.GetPossibleDestTiles(0, 3));
	RandomMovesPlayer white(testgame, ChessTeam::WHITE);
	white.AgrMovePiece();
	BoardVisualizerFunc(testgame);
}