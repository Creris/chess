#include "pieceknight.hpp"
#include <algorithm>

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

#include <iostream>

bool PieceKnight::canMove(Position toPos, const BoardState& state) const
{
	//If outside of the board or we are already staying there, invalid move
	if (!isInsideBoard(toPos, state) || toPos == position)
		return false;
	
	auto& moves = _allPossibleMoves();
	Position _ = { 100, 100 };
	Position diff = { toPos.first - position.first, toPos.second - position.second };

	auto it = std::find(moves.begin(), moves.end(), diff);
	auto& square = state.squares[toPos.first][toPos.second];

	return it != moves.end() && (square.type == PieceType::None ||
		square.type == PieceType::ShadowPawn || square.color != color);
}

PieceType PieceKnight::getType() const
{
	return PieceType::Knight;
}

std::vector<Position> PieceKnight::getAllAvailableMoves(const BoardState& state) const
{
	std::vector<Position> positions;

	//We will need to add to the initial position the offset, so this is a helper
	auto applyOffset = [](const Position& init, const Position& offset) -> Position {
		return { init.first + offset.first, init.second + offset.second };
	};

	//Go over all 8 possible moves by a knight(relative)
	for (auto& a : _allPossibleMoves()) {
		//apply the relative offset to our position
		Position off = applyOffset(position, a);
		//Check if we can actually move to that square, if we can
		//store that position as a possible move
		if (canMove(off, state))
			positions.push_back(off);
	}

	return positions;
}
