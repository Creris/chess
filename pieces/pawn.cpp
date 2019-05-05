#include "pawn.hpp"
#include <array>

#include <iostream>

bool PiecePawn::canMove(Position toPos, const BoardState& state) const
{
	if (!isInsideBoard(toPos, state) || toPos == position)
		return false;

	Position diff{ position.first - toPos.first, position.second - toPos.second };
	auto& square = state.squares[toPos.first][toPos.second];

	//Check diagonals, they have to be enemies
	if (std::abs(diff.second) == 1) {
		return std::abs(diff.first) == 1 && square.color != color &&
			square.type != PieceType::None;
	}

	int rankMultiplier = Color::Black == color ? -1 : 1;

	//Check front
	if (!diff.second && diff.first == -rankMultiplier)
		return square.type == PieceType::None || square.type == PieceType::ShadowPawn;

	//Check double front, if initial position
	if (!diff.second && diff.first == 2 * -rankMultiplier && isInitialPosition()) {
		Position sqPos = { toPos.first - rankMultiplier,
									toPos.second - rankMultiplier };
		return square.type == PieceType::None || square.type == PieceType::ShadowPawn
			&& canMove(sqPos, state);
	}

	return false;
}

PieceType PiecePawn::getType() const
{
	return PieceType::Pawn;
}

std::vector<Position> PiecePawn::getAllAvailableMoves(const BoardState& state) const
{
	std::vector<Position> positions;

	int rankMultiplier = Color::Black == color ? -1 : 1;

	std::array<Position, 4> allPositions = {
		Position{ position.first + 2 * rankMultiplier, position.second },
		Position{ position.first + rankMultiplier, position.second },
		Position{ position.first + rankMultiplier, position.second - 1 },
		Position{ position.first + rankMultiplier, position.second + 1 }
	};

	if (!isInitialPosition())
		allPositions[0] = { -1, -1 };

	for (auto& p : allPositions) {
		if (canMove(p, state))
			positions.push_back(p);
	}

	return positions;
}
