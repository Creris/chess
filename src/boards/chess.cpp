#include "../../include/boards/boardbuilder.hpp"
#include "../../include/boards/chess.hpp"
#include "../../include/pieces/generic.hpp"

#include "../../include/profiler.hpp"

void ChessBoard::initialize()
{
	ProfileDeclare;
	state.squares.clear();

	auto vec = std::vector<PieceStorage>{};
	vec.resize(8);
	state.squares.resize(8, vec);
	state.width = state.height = 8;
	_convertNulls();

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
	
	/*
	for (int i = 0; i < state.width; ++i) {
		//Distribute pawns
		addPiece({ 1, i }, PieceType::Pawn, Color::White);
		addPiece({ 6, i }, PieceType::Pawn, Color::Black);

		//Distribute others
		addPiece({ 0, i }, typeArray[i], Color::White);
		addPiece({ 7, i }, typeArray[i], Color::Black);
	}
	*/


	///*
	addPiece({ 1, 5 }, PieceType::King, Color::White);
	addPiece({ 7, 0 }, PieceType::King, Color::Black);
	addPiece({ 5, 4 }, PieceType::Rook, Color::Black);
	addPiece({ 3, 2 }, PieceType::Bishop, Color::Black);
	addPiece({ 2, 3 }, PieceType::Queen, Color::White);
	addPiece({ 6, 2 }, PieceType::Pawn, Color::White);
	//*/

	/*
	addPiece({ 6, 6 }, PieceType::King, Color::White);
	addPiece({ 1, 1 }, PieceType::Pawn, Color::White);
	addPiece({ 3, 0 }, PieceType::Rook, Color::White);
	addPiece({ 3, 7 }, PieceType::King, Color::Black);
	addPiece({ 3, 2 }, PieceType::Pawn, Color::Black);
	*/

	/*
	addPiece({ 5, 0 }, PieceType::Pawn, Color::White);
	addPiece({ 1, 6 }, PieceType::Pawn, Color::Black);
	addPiece({ 3, 4 }, PieceType::King, Color::White);
	addPiece({ 5, 6 }, PieceType::King, Color::Black);
	*/
	
	GenericBoard::initialize();
}
