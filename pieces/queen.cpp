#include "queen.hpp"
#include <array>

bool PieceQueen::canMove(Position toPos, const BoardState& state) const
{
	//If position is outside of the board, or its equal to the one
	//we are already standing at, its invalid move
	if (!isInsideBoard(toPos, state) || toPos == position)
		return false;

	Position diff{ position.first - toPos.first, position.second - toPos.second };

	//check if queen can move to this point ignoring obstructions
	if (!(diff.first == 0 || diff.second == 0 ||
			std::abs(diff.first) == std::abs(diff.second)))
		return false;

	//Copy position as the easiest way to do this is by modifying it
	//but this is const method so we cant modify our position iteratively
	auto posCopy = position;

	//Move from1 closer to to1 and from2 closer to to2 at the same time
	//If from1 == to1, only moves from2 towards to2, same vice versa
	//If neither is equal, moves on a diagonal
	auto moveCloser = [](auto& from1, auto to1, auto& from2, auto to2) {
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
		switch (state.squares[posCopy.first][posCopy.second].type) {
		case PieceType::None:
		case PieceType::ShadowPawn:
			//These are nonblocking
			continue;
		default:
			//Return whether the square is occupied by enemy AND the position is final
			//if the position isn't final, we can't reach the desired square
			//if it is final, we can step on it, granted if it is an enemy piece
			return state.squares[posCopy.first][posCopy.second].color != color
				&& toPos == posCopy;
		}
	}

	return true;
}

PieceType PieceQueen::getType() const
{
	return PieceType::Queen;
}

std::vector<Position> PieceQueen::getAllAvailableMoves(const BoardState& state) const
{
	std::vector<Position> positions;

	//A combination of rook's and bishop's getAllAvailableMoves.
	for (int i = 1; i < std::max(state.width, state.height); i++) {
		std::array<Position, 8> diagonals = {
			//same as bishop
			Position{ position.first + i, position.second + i },
			Position{ position.first + i, position.second - i },
			Position{ position.first - i, position.second + i },
			Position{ position.first - i, position.second - i },
			
			//same as rook
			Position{ position.first, position.second + i },
			Position{ position.first, position.second - i },
			Position{ position.first + i, position.second },
			Position{ position.first - i, position.second }
		};

		for (auto& p : diagonals) {
			if (canMove(p, state))
				positions.push_back(p);
		}
	}

	return positions;
}