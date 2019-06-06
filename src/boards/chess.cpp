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

	addPiece("c1", PieceType::King, Color::White);
	addPiece("a2", PieceType::Pawn, Color::White);
	addPiece("b2", PieceType::Pawn, Color::White);
	addPiece("c3", PieceType::Knight, Color::White);
	addPiece("c4", PieceType::Pawn, Color::White);
	addPiece("d5", PieceType::Queen, Color::White);
	addPiece("e3", PieceType::Bishop, Color::White);
	addPiece("e4", PieceType::Pawn, Color::White);
	addPiece("f1", PieceType::Bishop, Color::White);
	addPiece("f2", PieceType::Pawn, Color::White);
	addPiece("f3", PieceType::Knight, Color::White);
	addPiece("g2", PieceType::Pawn, Color::White);
	addPiece("h1", PieceType::Rook, Color::White);
	addPiece("h3", PieceType::Pawn, Color::White);

	addPiece("g8", PieceType::King, Color::Black);
	addPiece("a8", PieceType::Rook, Color::Black);
	addPiece("a4", PieceType::Pawn, Color::Black);
	addPiece("b7", PieceType::Pawn, Color::Black);
	addPiece("c8", PieceType::Bishop, Color::Black);
	addPiece("c6", PieceType::Queen, Color::Black);
	addPiece("d7", PieceType::Knight, Color::Black);
	addPiece("e8", PieceType::Rook, Color::Black);
	addPiece("e5", PieceType::Pawn, Color::Black);
	addPiece("f7", PieceType::Pawn, Color::Black);
	addPiece("f6", PieceType::Knight, Color::Black);
	addPiece("g7", PieceType::Bishop, Color::Black);
	addPiece("g6", PieceType::Pawn, Color::Black);
	addPiece("h7", PieceType::Pawn, Color::Black);


	/*
	for (int i = 0; i < state.width; ++i) {
		//Distribute pawns
		addPiece({ 1, i }, PieceType::Pawn, Color::White);
		addPiece({ 6, i }, PieceType::Pawn, Color::Black);

		//Distribute others
		addPiece({ 0, i }, typeArray[i], Color::White);
		addPiece({ 7, i }, typeArray[i], Color::Black);
	}
	//*/

	/*
	addPiece("c8", PieceType::King, Color::Black);

	addPiece("e3", PieceType::King, Color::White);
	addPiece("e5", PieceType::Bishop, Color::White);
	addPiece("d4", PieceType::Queen, Color::White);
	//*/

	/*
	addPiece("a7", PieceType::Bishop, Color::Black);
	addPiece("d6", PieceType::Bishop, Color::Black);

	addPiece("d8", PieceType::Rook, Color::Black);
	addPiece("h8", PieceType::Rook, Color::Black);

	addPiece("a1", PieceType::Rook, Color::White);
	addPiece("a5", PieceType::Rook, Color::White);

	addPiece("e4", PieceType::Queen, Color::White);
	addPiece("h4", PieceType::Queen, Color::White);
	addPiece("h1", PieceType::Queen, Color::White);

	addPiece("g7", PieceType::King, Color::Black);
	addPiece("b2", PieceType::King, Color::White);
	//*/

	/*
	addPiece("e1", PieceType::King, Color::White);
	addPiece("e8", PieceType::King, Color::Black);

	addPiece("a6", PieceType::Rook, Color::White);
	addPiece("f5", PieceType::Queen, Color::White);
	//*/

	/*
	addPiece("e1", PieceType::King, Color::White);
	addPiece("e8", PieceType::King, Color::Black);
	
	addPiece("a1", PieceType::Rook, Color::White);
	addPiece("h1", PieceType::Rook, Color::White);
	addPiece("d4", PieceType::Pawn, Color::White);

	addPiece("a8", PieceType::Rook, Color::Black);
	addPiece("h8", PieceType::Rook, Color::Black);

	addPiece("b2", PieceType::Pawn, Color::Black);
	//*/

	/*
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
