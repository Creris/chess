#include "../../include/pieces/king.hpp"
#include "../../include/boardstate.hpp"
#include "../../include/piecetype.hpp"

#include "../../include/boards/genericboard.hpp"

#include <array>
#include <vector>

bool _isEmpty(int8_t rank, int8_t fileBegin, int8_t fileEnd, const BoardState& state) {
	for (; fileBegin <= fileEnd; ++fileBegin) {
		Position pos = { rank, fileBegin };
		auto& pc = state.squares[pos.first][pos.second].piecePtr;
		if (pc && pc->getType() != PieceType::None)	return false;
	}
	return true;
}

bool _isAttacked(int8_t rank, int8_t fileBegin, int8_t fileEnd, int attIdx, const BoardState& state) {
	for (; fileBegin <= fileEnd; ++fileBegin) {
		Position pos = { rank, fileBegin };
		if (state.squares[pos.first][pos.second].threat[attIdx])	return true;
	}
	return false;
}

bool _isInitialPosition(Position pos, Color c) {
	return pos.second == 4
		&& (pos.first == 0 && c == Color::White) || (pos.first == 7 && c == Color::Black);
}

bool PieceKing::canMove(Position fromPos, Position toPos, const BoardState& state) const
{
	//Check if the move is valid and it is not a move to its own position
	if (!isInsideBoard(toPos, state) || toPos == fromPos)
		return false;

	auto& piece = *state.squares[toPos.first][toPos.second].piecePtr;
	auto type = piece.getType();
	Position diff = { toPos.first - fromPos.first, toPos.second - fromPos.second };
	auto attIdx = Color::Black == color ? 0 : 1;

	//Check castling, only if chess
	if (!std::abs(diff.first) && std::abs(diff.second) == 2 && state.type == BoardType::Chess) {
		//if we arent standing where we should be, or we moved before
		if (!_isInitialPosition(fromPos, color) || state.squares[fromPos.first][fromPos.second].didMove)
			return false;

		//left
		if (toPos.second == 2) {
			if (!_isEmpty(toPos.first, 1, 3, state)
				|| _isAttacked(toPos.first, 2, 4, attIdx, state))	return false;
			auto& sqInfo = state.squares[toPos.first][0];
			return !sqInfo.didMove && sqInfo.piecePtr && sqInfo.piecePtr->getType() == PieceType::Rook;
		}
		//right
		else if (toPos.second == 6) {
			if (!_isEmpty(toPos.first, 5, 6, state) 
				|| _isAttacked(toPos.first, 4, 6, attIdx, state))	return false;
			auto& sqInfo = state.squares[toPos.first][7];
			return !sqInfo.didMove && sqInfo.piecePtr && sqInfo.piecePtr->getType() == PieceType::Rook;
		}
		return false;
	}

	return (piece.getColor() != color || type == PieceType::None ||
			type == PieceType::ShadowPawn)
		&& std::abs(diff.first) < 2 && std::abs(diff.second) < 2;
}

PieceType PieceKing::getType() const
{
	return PieceType::King;
}

std::vector<Position> PieceKing::getAllAvailableMoves(Position fromPos,
													  const BoardState& state) const
{
	std::vector<Position> positions;

	//Check around the king
	for (int i = -1; i < 2; ++i) {
		for (int j = -1; j < 2; ++j) {
			//Ignore [0, 0] as he is already standing there
			if (i == j && !i)	continue;
			Position pos{ fromPos.first + i, fromPos.second + j };
			if (canMove(fromPos, pos, state))
				positions.push_back(pos);
		}
	}

	//Check castling options, CHESS ONLY!
	if (state.type == BoardType::Chess && _isInitialPosition(fromPos, color)) {
		auto attIdx = Color::Black == color ? 0 : 1;
		Position pos = { fromPos.first, fromPos.second - 2 };
		
		//Check left
		if (_isEmpty(fromPos.first, 1, 3, state)
			&& !_isAttacked(fromPos.first, 2, 3, attIdx, state)) {
			auto& sqInfo = state.squares[fromPos.first][0];
			if (!sqInfo.didMove && sqInfo.piecePtr && sqInfo.piecePtr->getType() == PieceType::Rook)
				positions.push_back(pos);
		}

		pos = { fromPos.first, fromPos.second + 2 };
		//Check right
		if (_isEmpty(fromPos.first, 5, 6, state)
			&& !_isAttacked(fromPos.first, 5, 6, attIdx, state)) {
			auto& sqInfo = state.squares[fromPos.first][7];
			if (!sqInfo.didMove && sqInfo.piecePtr && sqInfo.piecePtr->getType() == PieceType::Rook)
				positions.push_back(pos);
		}
	}

	return positions;
}

void PieceKing::moveAction(Position fromPos, Position toPos, BoardState& state) const
{
	Position diff{ fromPos.first - toPos.first, fromPos.second - toPos.second };
	if (std::abs(diff.second) == 2) {
		if (diff.second > 0) {
			//left
			auto pos = Position{ fromPos.first, 3 };
			state.squares[pos.first][pos.second] = std::move(state.squares[pos.first][0]);
			auto& square = state.squares[pos.first][0];
			square.piecePtr = newPieceByType(PieceType::None);
		}
		else {
			//right
			auto pos = Position{ fromPos.first, 5 };
			state.squares[pos.first][pos.second] = std::move(state.squares[pos.first][7]);
			auto& square = state.squares[pos.first][7];
			square.piecePtr = newPieceByType(PieceType::None);
		}
	}

	PieceGeneric::moveAction(fromPos, toPos, state);
}

std::vector<Position> PieceKing::getAllThreateningMoves(Position fromPos, const BoardState& state) const
{
	std::vector<Position> positions;

	for (auto& p : getAllAvailableMoves(fromPos, state)) {
		//Even if we cant move here right now, we are threatening this square
		//so use custom calculation
		Position diff = { fromPos.first - p.first, fromPos.second - p.second };
		if (std::abs(diff.second) > 1)	continue;

		if (!isInsideBoard(p, state))
			continue;

		auto& target = state.squares[p.first][p.second];
		if (target.piecePtr && target.piecePtr->getColor() != color)
			positions.push_back(p);
	}

	return positions;
}