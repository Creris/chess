#include "pieces/piecebuilder.hpp"

#include <iostream>
#include <iomanip>
#include <array>
#include <cctype>

#include "boards/boardbuilder.hpp"
#include "boards/chess.hpp"

int main() {
	BoardBuilder& b = BoardBuilder::getInstance();
	auto brd = b.build("Chess");
	std::cout << brd << "\n";
	brd = b.build("Chess");
	std::cout << brd << "\n";

	brd->preMove(nullptr, {});
	
	return 0;

	auto queen = newPieceByType(PieceType::Pawn, { 1, 2 }, Color::White);
	auto board = BoardState{ 8, 8, {} };
	board.squares.resize(8);
	for (auto& file : board.squares)
		file.resize(8, SquareInfo{ PieceInfo{ PieceType::None, Color::None }, {0, 0} });

	auto output = [](PieceType type) {
		switch (type) {
		case PieceType::King:
			return 'K';
		case PieceType::Queen:
			return 'Q';
		case PieceType::Bishop:
			return 'B';
		case PieceType::Knight:
			return 'N';
		case PieceType::Pawn:
			return 'P';
		case PieceType::Rook:
			return 'R';
		case PieceType::ShadowPawn:
			return 'S';
		}
		return ' ';
	};
	
	board.squares[1] = board.squares[6] = {
		{ PieceType::Pawn,	Color::Black },
		{ PieceType::Pawn,	Color::Black },
		{ PieceType::Pawn,	Color::Black },
		{ PieceType::Pawn,	Color::Black },
		{ PieceType::Pawn,	Color::Black },
		{ PieceType::Pawn,	Color::Black },
		{ PieceType::Pawn,	Color::Black },
		{ PieceType::Pawn,	Color::Black }
	};
	board.squares[0] = board.squares[7] = {
		{ PieceType::Rook,		Color::Black },
		{ PieceType::Knight,	Color::Black },
		{ PieceType::Bishop,	Color::Black },
		{ PieceType::Queen,		Color::Black },
		{ PieceType::King,		Color::Black },
		{ PieceType::Bishop,	Color::Black },
		{ PieceType::Knight,	Color::Black },
		{ PieceType::Rook,		Color::Black }
	};
	for (auto& squares : board.squares[0])	squares.piece.color = Color::White;
	for (auto& squares : board.squares[1])	squares.piece.color = Color::White;

	for (auto& squares : board.squares[4]) {
		squares.piece.color = Color::Black;
		squares.piece.type = PieceType::Rook;
	}

	auto printBoard = [](const auto& board) {
		for(int i = board.size() - 1; i >= 0; --i){
			auto& row = board[i];
			for (auto& col : row) {
				std::cout << ' ' << col << ' ';
			}
			std::cout << "\n\n";
		}
	};

	board.squares[queen->getPosition().first][queen->getPosition().second] = { queen->getType(), queen->getColor() };
	queen->move({ 3, 2 }, board);

	queen = newPieceByType(PieceType::Pawn, { 3, 1 }, Color::Black);
	board.squares[3][1] = { PieceType::Pawn, Color::Black };

	std::array<std::array<char, 8>, 8> graphicBoard = { ' ' };
	for (int rank = board.height - 1; rank >= 0; rank--) {
		for (int file = 0; file < board.width; file++) {
			auto square = board.squares[rank][file];
			auto c = output(square.piece.type);
			if (square.piece.color == Color::Black)
				c = std::tolower(c);
			graphicBoard[rank][file] = c;
		}
	}

	for (auto& square : queen->getAllAvailableMoves(board)) {
		graphicBoard[square.first][square.second] = '*';
	}
	
	printBoard(graphicBoard);

	return 0;
}