#include "../../include/pieces/generic.hpp"
#include "../../include/pieces/piecebuilder.hpp"
#include "../../include/boards/genericboard.hpp"
#include "../../include/boardstate.hpp"
#include "../../include/piecetype.hpp"

#include <array>
#include <vector>

void PieceGeneric::moveAction(Position fromPos, Position toPos, BoardState& state) const {
	if (!isInsideBoard(toPos, state))
		return;

	state.squares[toPos.first][toPos.second] = std::move(state.squares[fromPos.first][fromPos.second]);
	state.squares[fromPos.first][fromPos.second] = {};
	state.squares[fromPos.first][fromPos.second].piecePtr = newPieceByType(PieceType::None);
}

bool PieceGeneric::canMove(Position fromPos, Position toPos, const BoardState& state) const
{
	return false;
}

PieceType PieceGeneric::getType() const
{
	return PieceType::None;
}

Color PieceGeneric::getColor() const {
	return color;
}

bool PieceGeneric::isInsideBoard(Position pos, const BoardState& state) {
	return pos.first >= 0 && pos.second >= 0 &&
		pos.first < state.width && pos.second < state.height;
}

std::vector<PieceType> PieceGeneric::getUpgradeOptions() const {
	return {};
}

std::vector<Position> PieceGeneric::getAllAvailableMoves(Position fromPos, 
														 const BoardState& state) const
{
	return {};
}

std::vector<Position> PieceGeneric::getAllThreateningMoves(Position fromPos, const BoardState& state) const
{
	return getAllAvailableMoves(fromPos, state);
}

std::pair<bool, PieceStorage> PieceGeneric::move(Position fromPos,
												 Position toPos, BoardState& state) const {
	//If the move is not valid, ignore it and return empty state
	//to make sure the caller knows this was not successful
	if (this->canMove(fromPos, toPos, state)) {
		auto target = state.squares[toPos.first][toPos.second];
		this->moveAction(fromPos, toPos, state);
		return { true, target };
	}
	return { false, {} };
}