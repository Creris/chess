#include "pawn.hpp"
#include "piecebuilder.hpp"

#include <array>
#include <iostream>

bool PiecePawn::canMove(Position fromPos, Position toPos, const BoardState& state) const
{
	if (!isInsideBoard(toPos, state) || toPos == fromPos)
		return false;

	Position diff{ fromPos.first - toPos.first, fromPos.second - toPos.second };
	auto& piece = *state.squares[toPos.first][toPos.second].piecePtr;
	auto type = piece.getType();

	//Check diagonals, they have to be enemies
	if (std::abs(diff.second) == 1) {
		return std::abs(diff.first) == 1 && piece.getColor() != color &&
			type != PieceType::None;
	}

	int rankMultiplier = Color::Black == color ? -1 : 1;

	//Check front
	if (!diff.second && diff.first == -rankMultiplier)
		return type == PieceType::None || type == PieceType::ShadowPawn;

	//Check double front, if initial position
	if (!diff.second && diff.first == 2 * -rankMultiplier && isInitialPosition(fromPos)) {
		Position sqPos = { toPos.first - rankMultiplier,
									toPos.second };
		return type == PieceType::None || type == PieceType::ShadowPawn
			&& canMove(fromPos, sqPos, state);
	}

	return false;
}

PieceType PiecePawn::getType() const
{
	return PieceType::Pawn;
}

std::vector<Position> PiecePawn::getAllAvailableMoves(Position fromPos, 
													  const BoardState& state) const
{
	std::vector<Position> positions;

	int rankMultiplier = Color::Black == color ? -1 : 1;

	std::array<Position, 4> allPositions = {
		Position{ fromPos.first + 2 * rankMultiplier, fromPos.second },
		Position{ fromPos.first + rankMultiplier, fromPos.second },
		Position{ fromPos.first + rankMultiplier, fromPos.second - 1 },
		Position{ fromPos.first + rankMultiplier, fromPos.second + 1 }
	};

	if (!isInitialPosition(fromPos))
		allPositions[0] = { -1, -1 };

	for (auto& p : allPositions) {
		if (canMove(fromPos, p, state))
			positions.push_back(p);
	}

	return positions;
}

std::vector<PieceType> PiecePawn::getUpgradeOptions() const
{
	static std::vector<PieceType> types = {
		PieceType::Rook,
		PieceType::Bishop,
		PieceType::Queen,
		PieceType::Knight
	};

	return types;
}

void PiecePawn::moveAction(Position fromPos, Position toPos, BoardState& state)
{
	Position diff{ fromPos.first - toPos.first, fromPos.second - toPos.second };
	if (std::abs(diff.first) > 1) {
		auto pos = Position{ fromPos.first - diff.first / 2, fromPos.second };
		auto& square = state.squares[fromPos.first - diff.first / 2][fromPos.second];
		square.piecePtr = newPieceByType(PieceType::ShadowPawn, getColor());
	}

	PieceGeneric::moveAction(fromPos, toPos, state);
}
