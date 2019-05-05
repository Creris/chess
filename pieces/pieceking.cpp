#include "pieceking.hpp"

bool PieceKing::canMove(Position toPos, const BoardState& state) const
{
	//Check if the move is valid and it is not a move to its own position
	if (!isInsideBoard(toPos, state) || toPos == position)
		return false;

	auto& square = state.squares[toPos.first][toPos.second];
	Position diff = { toPos.first - position.first, toPos.second - position.second };
	return (square.color != color || square.type == PieceType::None ||
			square.type == PieceType::ShadowPawn)
		&& std::abs(diff.first) < 2 && std::abs(diff.second) < 2;
}

PieceType PieceKing::getType() const
{
	return PieceType::King;
}

std::vector<Position> PieceKing::getAllAvailableMoves(const BoardState& state) const
{
	std::vector<Position> positions;

	/* TODO: Make king recognize possible castling options.
			 Will require the king to be able to get rooks of the same color
			 and check if they moved yet and also check if he is in Check
			 or will be after castling, and also check if he has vision of the rook.
	*/
	//Check around the king
	for (int i = -1; i < 2; ++i) {
		for (int j = -1; j < 2; ++j) {
			//Ignore [0, 0] as he is already standing there
			if (i == j && !i)	continue;
			Position pos{ position.first + i, position.second + j };
			if (canMove(pos, state))
				positions.push_back(pos);
		}
	}

	return positions;
}
