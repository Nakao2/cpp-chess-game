#include "chess.h"
#include "board_visualizer.h"
#include "cpu_opponent.h"
#include "board_state_container.h"
#include "chess_history.h"
#include "chess_engine.h"
#include "log_duration2.h"

#include <iostream>
#include <chrono>
#include <thread>
#include <vector>
#include <map>

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
	game1.EmptyBoard();
	game1.PutPieceInPosition({ ChessPiece::KING, ChessTeam::WHITE, false }, 7, 4);
	game1.PutPieceInPosition({ ChessPiece::ROOK, ChessTeam::WHITE, false }, 7, 0);
	game1.PutPieceInPosition({ ChessPiece::ROOK, ChessTeam::WHITE, false }, 7, 7);
	game1.PutPieceInPosition({ ChessPiece::KING, ChessTeam::BLACK, false }, 0, 3);
	game1.PutPieceInPosition({ ChessPiece::ROOK, ChessTeam::BLACK, false }, 0, 0);
	game1.PutPieceInPosition({ ChessPiece::ROOK, ChessTeam::BLACK, false }, 0, 7);
	RandomMovesPlayer white(game1, ChessTeam::WHITE);
	RandomMovesPlayer black(game1, ChessTeam::BLACK);
	BoardVisualizerFunc(game1);
	BoardStateContainer boards;
	for (int i = 1; i < 10; ++i) {
		cout << "Turn " << i++ << ": \n";
		white.MovePiece();
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
	BoardVisualizerFunc(boards.GetBoardState(6));
}

void PlayGameWithCPUS2() {
	using namespace std::chrono_literals;
	ChessWithHistory game1;
	RandomMovesPlayer white(game1, ChessTeam::WHITE);
	RandomMovesPlayer black(game1, ChessTeam::BLACK);
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
	cout << "\n\n\n\n";
	const BoardStateContainer& boards = game1.GetHistory();
	cout << boards.GetSize() << "\n\n\n";
	Chess board = game1.GetBoardState(13);
	BoardVisualizerFunc(board);
	board.EmptyBoard();
	BoardVisualizerFunc(board);
}

void EnpassantTest() {
	using namespace std::chrono_literals;
	Chess game1(8, 8);
	game1.PutPieceInPosition({ ChessPiece::KING, ChessTeam::BLACK, false }, 0, 0);
	game1.PutPieceInPosition({ ChessPiece::KING, ChessTeam::WHITE, false }, 7, 7);
	game1.PutPieceInPosition({ ChessPiece::PAWN, ChessTeam::BLACK, false }, 2, 3);
	game1.PutPieceInPosition({ ChessPiece::PAWN, ChessTeam::BLACK, false }, 2, 5);
	game1.PutPieceInPosition({ ChessPiece::PAWN, ChessTeam::WHITE, false }, 4, 4);
	game1.MovePiece({ 7, 7 }, { 6, 7 });
	BoardVisualizerFunc(game1);
	game1.MovePiece({ 2, 3 }, { 4, 3 });
	BoardVisualizerFunc(game1);
	PrintDeque(game1.GetPossibleDestTiles(4, 4));
}

void PawnPromotionTest() {
	ChessWithHistory game1(8 ,8);
	game1.PutPieceInPosition({ ChessPiece::KING, ChessTeam::BLACK, false }, 0, 0);
	game1.PutPieceInPosition({ ChessPiece::KING, ChessTeam::WHITE, false }, 7, 7);
	for (int i = 0; i < 8; ++i) {
		game1.PutPieceInPosition({ ChessPiece::PAWN, ChessTeam::BLACK, false }, 2, i);
		game1.PutPieceInPosition({ ChessPiece::PAWN, ChessTeam::WHITE, false }, 5, i);
	}
	RandomMovesPlayer white(game1, ChessTeam::WHITE);
	RandomMovesPlayer black(game1, ChessTeam::BLACK);
	for (int i = 1; i < 102; ++i) {
		cout << "Turn " << i++ << ": \n";
		white.MovePiece();
		if (game1.PawnPromotion()) {
			game1.PawnPromotion(ChessPiece::QUEEN);
		}
		BoardVisualizerFunc(game1);
		cout << "Turn " << i << ": \n";
		black.MovePiece();
		if (game1.PawnPromotion()) {
			game1.PawnPromotion(ChessPiece::QUEEN);
		}
		BoardVisualizerFunc(game1);
	}
}

void PlayGameHotSeat() {
	Chess game1(8, 8);
	for (int i = 0; i < 8; ++i) {
		game1.PutPieceInPosition({ ChessPiece::PAWN, ChessTeam::BLACK, false }, 2, i);
		game1.PutPieceInPosition({ ChessPiece::PAWN, ChessTeam::WHITE, false }, 5, i);
	}
	game1.PutPieceInPosition({ ChessPiece::KING, ChessTeam::BLACK, false }, 0, 0);
	game1.PutPieceInPosition({ ChessPiece::KING, ChessTeam::WHITE, false }, 4, 7);
	game1.PutPieceInPosition({ ChessPiece::ROOK, ChessTeam::BLACK, false }, 4, 0);
	BoardVisualizerFunc(game1);
	for (int i = 1; i < 100; ++i) {
		int i_pos1, i_pos2;
		int o_pos1, o_pos2;
		cin >> i_pos1 >> i_pos2;
		cin >> o_pos1 >> o_pos2;
		game1.MovePiece({ i_pos1, i_pos2 }, { o_pos1, o_pos2 });
		BoardVisualizerFunc(game1);
	}
}

void ChessEngineTest() {
	Chess board;
	int n_in, m_in;
	int n_out, m_out;
	MoveData move;
	for (int i = 0; i < 38; ++i) {
		BoardVisualizerFunc(board);
		std::cin >> n_in >> m_in;
		std::cin >> n_out >> m_out;
		board.MovePiece({ n_in, m_in }, { n_out, m_out });
		BoardVisualizerFunc(board);
		move = PlayMove(board, ChessTeam::BLACK, 5);
		board.MovePiece(move.start, move.end);
	}
}
void ChessEngineTest2() {
	Chess board;
	int n_in, m_in;
	int n_out, m_out;
	FullMoveData move;
	for (int i = 0; i < 50; ++i) {
		BoardVisualizerFunc(board);
		std::cin >> n_in >> m_in;
		std::cin >> n_out >> m_out;
		board.MovePiece({ n_in, m_in }, { n_out, m_out });
		if (board.PawnPromotion()) {
			board.PawnPromotion(ChessPiece::QUEEN);
		}
		BoardVisualizerFunc(board);
		move = PlayMoveOP(board, ChessTeam::BLACK, 5);
		board.MovePiece(move.own_move.start, move.own_move.end);
		if (move.promotion_data.first) {
			board.PawnPromotion(move.promotion_data.second);
		}
	}
}

void Test2() {
	Chess board;
	int n_in, m_in;
	int n_out, m_out;
	for (int i = 0; i < 4; ++i) {
		BoardVisualizerFunc(board);
		std::cin >> n_in >> m_in;
		std::cin >> n_out >> m_out;
		board.MovePiece({ n_in, m_in }, { n_out, m_out });
		BoardVisualizerFunc(board);
		std::cin >> n_in >> m_in;
		std::cin >> n_out >> m_out;
		board.MovePiece({ n_in, m_in }, { n_out, m_out });
	}
	BoardVisualizerFunc(board);
	PrintDeque(board.GetPossibleDestTiles(6, 0));
}

struct PieceImage {
	ChessPiece piece;
	ChessTeam team;
};

/*  Chess board;
	MoveData move;
	BoardVisualizerFunc(board);
	for (int i = 0; i < 50; ++i) {
		int n_in, m_in, n_out, m_out;
		std::cin >> n_in >> m_in >> n_out >> m_out;
		board.MovePiece({ n_in, m_in }, { n_out, m_out });
		BoardVisualizerFunc(board);
		{
			LogDuration log2("1", std::cout);
			std::cout << '\n';
			move = PlayMove(board, ChessTeam::BLACK, 5);
		}
		{
			LogDuration log2("2", std::cout);
			std::cout << '\n';
			move = PlayMoveOP(board, ChessTeam::BLACK, 5);
		}
		board.ForceMove(move.start, move.end);
		BoardVisualizerFunc(board);
	}*/

int main() {
	std::cout << std::thread::hardware_concurrency();
}