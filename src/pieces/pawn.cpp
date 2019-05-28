#include "../../include/pieces/pawn.hpp"
#include "../../include/pieces/piecebuilder.hpp"

#include "../../include/boardstate.hpp"
#include "../../include/piecetype.hpp"

#include <array>
#include <vector>

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
		return type == PieceType::None;

	//Check double front, if initial position
	if (!diff.second && diff.first == 2 * -rankMultiplier && isInitialPosition(fromPos)) {
		Position sqPos = { toPos.first - rankMultiplier,
									toPos.second };
		return type == PieceType::None && canMove(fromPos, sqPos, state);
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

std::vector<Position> PiecePawn::getAllThreateningMoves(Position fromPos, const BoardState& state) const
{
	std::vector<Position> positions;

	int rankMultiplier = Color::Black == color ? -1 : 1;

	std::array<Position, 2> allPositions = {
		Position{ fromPos.first + rankMultiplier, fromPos.second - 1 },
		Position{ fromPos.first + rankMultiplier, fromPos.second + 1 }
	};

	for (auto& p : allPositions) {
		//Even if we cant move here right now, we are threatening this square
		//so use custom calculation
		if (!isInsideBoard(p, state))
			continue;

		auto& target = state.squares[p.first][p.second];
		if (target.piecePtr && target.piecePtr->getColor() != color)
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

void PiecePawn::moveAction(Position fromPos, Position toPos, BoardState& state) const
{
	Position diff{ fromPos.first - toPos.first, fromPos.second - toPos.second };
	if (std::abs(diff.first) > 1) {
		auto pos = Position{ fromPos.first - diff.first / 2, fromPos.second };
		auto& square = state.squares[fromPos.first - diff.first / 2][fromPos.second];
		square.piecePtr = newPieceByType(PieceType::ShadowPawn, getColor());
	}

	PieceGeneric::moveAction(fromPos, toPos, state);
}

inline Position _getPawnPosFromShadow(Position shadowPos, const BoardState& state) {
	if (shadowPos.first == 5)
		return { shadowPos.first - 1, shadowPos.second };
	else if (shadowPos.first == 2)
		return { shadowPos.first + 1, shadowPos.second };
	return { -1, -1 };
}

inline PieceStorage _getPawnFromShadow(Position shadowPos, const BoardState& state) {
	auto pos = _getPawnPosFromShadow(shadowPos, state);
	if (pos.first != -1 || pos.second != -1)
		return state.squares[pos.first][pos.second];
	return {};
}

std::pair<bool, PieceStorage> PiecePawn::move(Position fromPos, Position toPos, BoardState& state) const
{
	//If the move is not valid, ignore it and return empty state
	//to make sure the caller knows this was not successful
	if (this->canMove(fromPos, toPos, state)) {
		auto target = state.squares[toPos.first][toPos.second];
		this->moveAction(fromPos, toPos, state);

		//If we are taking a shadow pawn, return the pawn it is shadowing instead
		//of the shadow itself
		if (target.piecePtr && target.piecePtr->getType() == PieceType::ShadowPawn) {
			target = _getPawnFromShadow(toPos, state);
			auto realPawnPos = _getPawnPosFromShadow(toPos, state);

			//dont forget to capture the real pawn from the board too, otherwise
			//we would capture the shadow and leave this one on board
			state.squares[realPawnPos.first][realPawnPos.second] = {};
			state.squares[realPawnPos.first][realPawnPos.second].piecePtr = newPieceByType(PieceType::None);
		}
		return { true, target };
	}
	return { false, {} };
}
