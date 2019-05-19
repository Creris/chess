#include "boardbuilder.hpp"
#include "chess.hpp"

#include "../pieces/generic.hpp"

void ChessBoard::initialize()
{
	//If the state is not exactly correct, reinitialize
	if (state.width != state.height && state.width != 8) {
		auto vec = std::vector<PieceStorage>{};
		vec.resize(8);
		state.squares.resize(8, vec);
		state.width = state.height = 8;
		_convertNulls();
	}

	std::array<PieceType, 8> typeArray = {
		PieceType::Rook,
		PieceType::Knight,
		PieceType::Bishop,
		PieceType::Queen,
		PieceType::King,
		PieceType::Bishop,
		PieceType::Knight,
		PieceType::Rook
	};

	for (int i = 0; i < state.width; ++i) {
		//Distribute pawns
		addPiece({ 1, i }, PieceType::Pawn, Color::White);
		addPiece({ 6, i }, PieceType::Pawn, Color::Black);

		//Distribute others
		addPiece({ 0, i }, typeArray[i], Color::White);
		addPiece({ 7, i }, typeArray[i], Color::Black);
	}

	state.squares[6][3] = {};
}
