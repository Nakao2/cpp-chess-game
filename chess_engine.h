#pragma once

#include "chess.h"
#include "piece_value_calculator.h"

#include <tuple>
#include <vector>
#include <algorithm>
#include <list>
#include <deque>
#include <stack>

struct MoveData {
	std::pair<int, int> start;
	std::pair<int, int> end;
};

bool operator!=(const BoardTile& first, const BoardTile& second) {
	if (first.piece_type != second.piece_type || first.piece_team != second.piece_team ||
		first.has_moved != second.has_moved) {
		return true;
	}
	return false;
}

void UpdatePieces(std::vector<std::pair<int, int>>& source, const Chess& board, ChessTeam team) {
	source.clear();
	for (int n = 0; n < board.GetDimensions().first; ++n) {
		for (int m = 0; m < board.GetDimensions().second; ++m) {
			if (board.LookUp(n, m).piece_team == team) {
				source.push_back({ n, m });
			}
		}
	}
}

// Add pawn promotion
uint32_t GenerateMoves(std::stack<std::pair<int, Chess>>& stack, ChessTeam team, bool write_negative_values) {
	uint32_t num_moves_generated = 0;
	Chess board_copy(stack.top().second);
	std::vector<std::pair<int, int>> own_pieces;
	std::deque<std::pair<int, int>> moves;
	UpdatePieces(own_pieces, board_copy, team);
	for (auto [n_in, m_in] : own_pieces) {
		moves = board_copy.GetPossibleDestTiles(n_in, m_in);
		for (auto [n_out, m_out] : moves) {
			Chess board_output(board_copy);
			int value_change = 0;
			BoardTile dest_tile = board_output.LookUp(n_out, m_out);
			value_change += GivePieceValue(dest_tile.piece_type);
			if (board_output.LookUp(n_in, m_in).piece_type == ChessPiece::PAWN && std::abs(m_in - m_out) == 1 &&
				dest_tile.piece_team == ChessTeam::NEUTRAL) {
				value_change += 1;
			}
			board_output.ForceMove({ n_in, m_in }, { n_out, m_out });
			if (write_negative_values) {
				value_change = -1 * value_change;
			}
			stack.push({ value_change, std::move(board_output) });
			++num_moves_generated;
		}
	}
	return num_moves_generated;
}

int Refresh_Values(uint8_t depth) {
	if (depth % 2 == 0) {
		return INT32_MIN / 2;
	}
	else {
		return INT32_MAX / 2;
	}
}

// Add correct castling recognition
MoveData GetMoveDataFromBoards(const Chess& board_start, const Chess& board_end) {
	MoveData output;
	ChessTeam team = board_start.WhoseMove();
	for (int n = 0; n < board_start.GetDimensions().first; ++n) {
		for (int m = 0; m < board_start.GetDimensions().second; ++m) {
			if (board_start.LookUp(n, m) != board_end.LookUp(n, m)) {
				if (board_end.LookUp(n, m).piece_type == ChessPiece::EMPTY &&
					board_start.LookUp(n, m).piece_team == team) {
					output.start = { n, m };
				}
				else {
					output.end = { n, m };
				}
			}
		}
	}
	return output;
}

MoveData PlayMove(Chess board, ChessTeam team, uint8_t depth) {
	if (depth == 0 || team == ChessTeam::NEUTRAL) {
		return {};
	}
	std::vector<std::pair<int, Chess>> first_moves;
	std::vector<std::pair<int, int>> own_pieces;
	std::deque<std::pair<int, int>> moves;
	UpdatePieces(own_pieces, board, team);
	for (auto [n_in, m_in] : own_pieces) {
		moves = board.GetPossibleDestTiles(n_in, m_in);
		for (auto [n_out, m_out] : moves) {
			Chess board_output(board);
			int value_change = 0;
			BoardTile dest_tile = board_output.LookUp(n_out, m_out);
			value_change += GivePieceValue(dest_tile.piece_type);
			if (board_output.LookUp(n_in, m_in).piece_type == ChessPiece::PAWN && std::abs(m_in - m_out) &&
				dest_tile.piece_team == ChessTeam::NEUTRAL) {
				value_change += 1;
			}
			board_output.ForceMove({ n_in, m_in }, { n_out, m_out });
			first_moves.push_back({ value_change, std::move(board_output) });
		}
	}

	ChessTeam enemy_team = (team == ChessTeam::WHITE) ? ChessTeam::BLACK : ChessTeam::WHITE;
	std::stack<std::pair<int, Chess>> boards;
	std::vector<int> depth_values(depth, 0);
	for (uint8_t i = 1; i < depth_values.size(); ++i) {
		depth_values[i] = Refresh_Values(i);
	}
	std::vector<int> rem_depth(depth, 0);
	ChessTeam team_tmp;
	bool flag_tmp = true;
	for (size_t num = 0; num < first_moves.size(); ++num) {
		uint8_t cur_depth = 0;
		boards.push(first_moves[num]);
		rem_depth[0] = 1;
		depth_values[0] = INT32_MIN;
		while (rem_depth[0] > 0) {
			if (cur_depth < depth - 1 && rem_depth[cur_depth] != 0) {
				++cur_depth;
				if (cur_depth % 2 == 1) {
					team_tmp = enemy_team;
					flag_tmp = true;
				}
				else {
					team_tmp = team;
					flag_tmp = false;
				}
				rem_depth[cur_depth] = GenerateMoves(boards, team_tmp, flag_tmp);
			}
			else {
				if (cur_depth == depth - 1) {
					while (rem_depth[cur_depth] != 0) {
						if (cur_depth % 2 == 0) {
							depth_values[cur_depth] = std::max(depth_values[cur_depth], boards.top().first);
						}
						else {
							depth_values[cur_depth] = std::min(depth_values[cur_depth], boards.top().first);
						}
						--rem_depth[cur_depth];
						boards.pop();
					}
				}
				--cur_depth;
				int val_tmp = boards.top().first + depth_values[cur_depth + 1];
				if (cur_depth % 2 == 0) {
					depth_values[cur_depth] = std::max(val_tmp, depth_values[cur_depth]);
				}
				else {
					depth_values[cur_depth] = std::min(val_tmp, depth_values[cur_depth]);
				}
				depth_values[cur_depth + 1] = Refresh_Values(cur_depth + 1);
				--rem_depth[cur_depth];
				boards.pop();
			}
		}
		first_moves[num].first = depth_values[0];
	}

	int best_value = INT32_MIN;
	std::cout << '\n' << "All found values - ";
	for (const std::pair<int, Chess>& move : first_moves) {
		std::cout << move.first << ' ';
		if (move.first > best_value) {
			best_value = move.first;
		}
	}
	std::cout << '\n';
	Chess best_move;
	for (const std::pair<int, Chess>& move : first_moves) {
		if (move.first == best_value) {
			best_move = move.second;
			std::pair<int, int> output_pos = GetMoveDataFromBoards(board, best_move).end;
			if (best_move.LookUp(output_pos.first, output_pos.second).piece_type == ChessPiece::PAWN) {
				break;
			}
		}
	}
	std::cout << "Played a move with a value of " << best_value << '\n';
	return GetMoveDataFromBoards(board, best_move);
}

struct FullMoveData {
	MoveData own_move;
	bool has_moved;
	std::pair<BoardTile, std::pair<int, int>> captured_piece;
	std::pair<bool, std::pair<int, int>> en_passant;
	std::pair<bool, ChessPiece> promotion_data;
};

// Castling reverse does not work properly if rook is initialy not at the border of the board
void ReverseMove(Chess& board, const FullMoveData& data) {
	BoardTile moved_piece = board.LookUp(data.own_move.end.first, data.own_move.end.second);
	board.PutPieceInPosition({ ChessPiece::EMPTY, ChessTeam::NEUTRAL, false }, data.own_move.end.first, data.own_move.end.second);
	moved_piece.has_moved = data.has_moved;
	if (data.promotion_data.first) {
		moved_piece.piece_type = ChessPiece::PAWN;
	}
	board.PutPieceInPosition(moved_piece, data.own_move.start.first, data.own_move.start.second);
	board.PutPieceInPosition(data.captured_piece.first, data.captured_piece.second.first, data.captured_piece.second.second);
	board.SetEnpassantData(data.en_passant);
	board.SwitchTurnSequence();
	if (moved_piece.piece_type == ChessPiece::KING &&
		std::abs(data.own_move.start.second - data.own_move.end.second) == 2) {
		int pos = 0;
		if (data.own_move.start.second < data.own_move.end.second) {
			pos = board.GetDimensions().second - 1;
		}
		board.PutPieceInPosition({ ChessPiece::ROOK, board.WhoseMove(), false}, data.own_move.start.first, pos);
		board.PutPieceInPosition({ ChessPiece::EMPTY, ChessTeam::NEUTRAL, false }, data.own_move.start.first,
								   (data.own_move.start.second + data.own_move.end.second) / 2);
	}
}

uint32_t GenerateMovesOP(const Chess& board, std::vector<std::pair<int, FullMoveData>>& stack, bool write_negative) {
	uint32_t num_moves_generated = 0;
	FullMoveData output;
	ChessTeam team = board.WhoseMove();
	std::vector<std::pair<int, int>> pieces;
	std::deque<std::pair<int, int>> moves;
	int8_t multiplier = 1;
	if (write_negative) {
		multiplier = -1;
	}
	UpdatePieces(pieces, board, team);
	for (auto [n_in, m_in] : pieces) {
		moves = board.GetPossibleDestTiles(n_in, m_in);
		for (auto [n_out, m_out] : moves) {
			int value_change = 0;
			BoardTile dest_tile = board.LookUp(n_out, m_out);
			output.own_move.start = { n_in, m_in };
			output.own_move.end = { n_out, m_out };
			output.captured_piece = { dest_tile, {n_out, m_out} };
			output.has_moved = board.LookUp(n_in, m_in).has_moved;
			output.en_passant = board.GetEnpassantData();
			output.promotion_data.first = false;

			if (board.LookUp(n_in, m_in).piece_type == ChessPiece::PAWN &&
				dest_tile.piece_type == ChessPiece::EMPTY &&
				std::abs(m_in - m_out) == 1) {

				value_change += 1;
				output.captured_piece = { board.LookUp(n_in, m_out), {n_in, m_out} };
			}
			value_change += GivePieceValue(dest_tile.piece_type);
			if (board.LookUp(n_in, m_in).piece_type == ChessPiece::PAWN &&
				(n_out == 0 || n_out == board.GetDimensions().first - 1)) {

				output.promotion_data.first = true;

				output.promotion_data.second = ChessPiece::QUEEN;
				value_change += 8;
				stack.push_back({ value_change * multiplier, output });
				value_change -= 8;

				output.promotion_data.second = ChessPiece::KNIGHT;
				value_change += 2;
				stack.push_back({ value_change * multiplier, output });
				value_change -= 2;

				output.promotion_data.second = ChessPiece::BISHOP;
				value_change += 2;
				stack.push_back({ value_change * multiplier, output });
				value_change -= 2;

				output.promotion_data.second = ChessPiece::ROOK;
				value_change += 4;
				stack.push_back({ value_change * multiplier, output });
				value_change -= 4;

				num_moves_generated += 4;
			}
			else {
				++num_moves_generated;
				stack.push_back({ value_change * multiplier, output });
			}
		}
	}
	return num_moves_generated;
}

// Add promotion data output
FullMoveData PlayMoveOP(Chess board, ChessTeam team, uint8_t depth) {
	std::vector<std::pair<int, FullMoveData>> moves_data;
	moves_data.reserve(uint32_t(depth) * 50);
	std::vector<std::pair<int, FullMoveData>> first_moves;
	GenerateMovesOP(board, first_moves, false);

	if (depth > 1) {

		std::vector<int> depth_values(depth, 0);
		for (uint8_t i = 1; i < depth_values.size(); ++i) {
			depth_values[i] = Refresh_Values(i);
		}
		std::vector<int> rem_depth(depth, 0);
		FullMoveData tmp;

		for (auto& first_move : first_moves) {
			moves_data.push_back(first_move);
			depth_values[0] = INT32_MIN / 2;
			rem_depth[0] = 1;
			uint8_t cur_depth = 0;
			while (rem_depth[0] > 0) {
				if (cur_depth < depth - 1 && rem_depth[cur_depth] != 0) {
					++cur_depth;
					tmp = moves_data.back().second;
					board.ForceMove(tmp.own_move.start, tmp.own_move.end);
					if (tmp.promotion_data.first) {
						board.PawnPromotion(tmp.promotion_data.second);
					}
					rem_depth[cur_depth] = GenerateMovesOP(board, moves_data, board.WhoseMove() != team);
				}
				else {
					if (cur_depth == depth - 1) {
						while (rem_depth[cur_depth] != 0) {
							if (cur_depth % 2 == 0) {
								depth_values[cur_depth] = std::max(depth_values[cur_depth], moves_data.back().first);
							}
							else {
								depth_values[cur_depth] = std::min(depth_values[cur_depth], moves_data.back().first);
							}
							--rem_depth[cur_depth];
							moves_data.pop_back();
						}
					}
					--cur_depth;
					int val_tmp = moves_data.back().first + depth_values[cur_depth + 1];
					if (cur_depth % 2 == 0) {
						depth_values[cur_depth] = std::max(val_tmp, depth_values[cur_depth]);
					}
					else {
						depth_values[cur_depth] = std::min(val_tmp, depth_values[cur_depth]);
					}
					depth_values[cur_depth + 1] = Refresh_Values(cur_depth + 1);
					--rem_depth[cur_depth];
					ReverseMove(board, moves_data.back().second);
					moves_data.pop_back();
				}
			}
			first_move.first = depth_values[0];
		}
	}

	int best_value = INT32_MIN;
	for (const std::pair<int, FullMoveData>& move : first_moves) {
		if (best_value < move.first) {
			best_value = move.first;
		}
	}

	FullMoveData output;
	for (const std::pair<int, FullMoveData>& move : first_moves) {
		if (move.first == best_value) {
			MoveData own_move = move.second.own_move;
			output = move.second;
			if (board.LookUp(own_move.start.first, own_move.start.second).piece_type == ChessPiece::PAWN) {
				break;
			}
		}
	}
	return output;
}