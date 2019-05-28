#include "../../include/pieces/knight.hpp"
#include "../../include/boardstate.hpp"
#include "../../include/piecetype.hpp"

#include <array>
#include <algorithm>
#include <vector>

const std::vector<Position>& PieceKnight::_allPossibleMoves() const
{
	static std::vector<Position> vec = {
		{1, 2}, {2, 1},
		{-1, 2}, {-2, 1},
		{-1, -2}, {-2, -1},
		{1, -2}, {2, -1}
	};
	return vec;
}

bool PieceKnight::canMove(Position fromPos, Position toPos, const BoardState& state) const
{
	//If outside of the board or we are already staying there, invalid move
	if (!isInsideBoard(toPos, state) || toPos == fromPos)
		return false;
	
	auto& moves = _allPossibleMoves();
	Position _ = { 100, 100 };
	Position diff = { toPos.first - fromPos.first, toPos.second - fromPos.second };

	auto it = std::find(moves.begin(), moves.end(), diff);
	auto& square = state.squares[toPos.first][toPos.second];
	auto& piece = square.piecePtr;
	return it != moves.end() && (piece->getType() == PieceType::None ||
		piece->getType() == PieceType::ShadowPawn || piece->getColor() != color);
}

PieceType PieceKnight::getType() const
{
	return PieceType::Knight;
}

std::vector<Position> PieceKnight::getAllAvailableMoves(Position fromPos,
														const BoardState& state) const
{
	std::vector<Position> positions;

	//We will need to add to the initial position the offset, so this is a helper
	auto applyOffset = [](const Position& init, const Position& offset) -> Position {
		return { init.first + offset.first, init.second + offset.second };
	};

	//Go over all 8 possible moves by a knight(relative)
	for (auto& a : _allPossibleMoves()) {
		//apply the relative offset to our position
		Position off = applyOffset(fromPos, a);
		//Check if we can actually move to that square, if we can
		//store that position as a possible move
		if (canMove(fromPos, off, state))
			positions.push_back(off);
	}

	return positions;
}
