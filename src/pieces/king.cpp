#include "king.hpp"

bool PieceKing::canMove(Position fromPos, Position toPos, const BoardState& state) const
{
	//Check if the move is valid and it is not a move to its own position
	if (!isInsideBoard(toPos, state) || toPos == fromPos)
		return false;

	auto& piece = *state.squares[toPos.first][toPos.second].piecePtr;
	auto type = piece.getType();
	Position diff = { toPos.first - fromPos.first, toPos.second - fromPos.second };
	auto attIdx = Color::Black == color ? 0 : 1;

	return (piece.getColor() != color || type == PieceType::None ||
			type == PieceType::ShadowPawn)
		&& std::abs(diff.first) < 2 && std::abs(diff.second) < 2
		//Can't move towards a check
		&& !state.squares[toPos.first][toPos.second].threat[attIdx].size();
}

PieceType PieceKing::getType() const
{
	return PieceType::King;
}

std::vector<Position> PieceKing::getAllAvailableMoves(Position fromPos,
													  const BoardState& state) const
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
			Position pos{ fromPos.first + i, fromPos.second + j };
			if (canMove(fromPos, pos, state))
				positions.push_back(pos);
		}
	}

	return positions;
}
