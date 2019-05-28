#include "../../include/pieces/bishop.hpp"
#include "../../include/boardstate.hpp"
#include "../../include/piecetype.hpp"

#include <vector>
#include <array>

bool PieceBishop::canMove(Position fromPos, Position toPos, const BoardState& state) const
{
	//SAME CODE AS QUEEN, ONLY DIFFERENCE IS THAT THE CHECK WHICH IGNORES
	//OBSTRUCTIONS OMITS STRAIGHTS.

	//If position is outside of the board, or its equal to the one
	//we are already standing at, its invalid move
	if (!isInsideBoard(toPos, state) || toPos == fromPos)
		return false;

	Position diff{ fromPos.first - toPos.first, fromPos.second - toPos.second };

	//check if queen can move to this point ignoring obstructions
	if (!(std::abs(diff.first) == std::abs(diff.second)))
		return false;

	//Copy position as the easiest way to do this is by modifying it
	//but this is const method so we cant modify our position iteratively
	auto posCopy = fromPos;

	//Move from1 closer to to1 and from2 closer to to2 at the same time
	//If from1 == to1, only moves from2 towards to2, same vice versa
	//If neither is equal, moves on a diagonal
	auto moveCloser = [](auto & from1, auto to1, auto & from2, auto to2) {
		bool equal = from1 == to1 && from2 == to2;
		if (to1 > from1)		from1++;
		else if (to1 < from1)	from1--;

		if (to2 > from2)		from2++;
		else if (to2 < from2)	from2--;
		return !equal;
	};

	//Raycast through the board towards the square we want to check
	//if we can move to one square at a time and check if it is occupied
	//by anything that can block us
	while (moveCloser(posCopy.first, toPos.first, posCopy.second, toPos.second)) {
		auto& piece = *state.squares[posCopy.first][posCopy.second].piecePtr;
		switch (piece.getType()) {
		case PieceType::None:
		case PieceType::ShadowPawn:
			//These are nonblocking
			continue;
		default:
			//Return whether the square is occupied by enemy AND the position is final
			//if the position isn't final, we can't reach the desired square
			//if it is final, we can step on it, granted if it is an enemy piece
			return piece.getColor() != color
				&& toPos == posCopy;
		}
	}

	return true;
}

PieceType PieceBishop::getType() const
{
	return PieceType::Bishop;
}

std::vector<Position> PieceBishop::getAllAvailableMoves(Position fromPos,
														const BoardState& state) const
{
	std::vector<Position> positions;

	//Check 1, 2, until max(width, height) squares away from bishop
	//Checks a lot of out of bounds but still less squares than squares on board
	for (int i = 1; i < std::max(state.width, state.height); i++) {
		//4 diagonals = 4 possible positions
		std::array<Position, 4> diagonals = {
			Position{ fromPos.first + i, fromPos.second + i },
			Position{ fromPos.first + i, fromPos.second - i },
			Position{ fromPos.first - i, fromPos.second + i },
			Position{ fromPos.first - i, fromPos.second - i }
		};

		for (auto& p : diagonals) {
			if (canMove(fromPos, p, state))
				positions.push_back(p);
		}
	}

	return positions;
}
