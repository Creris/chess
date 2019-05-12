#include "pieces/piecebuilder.hpp"

#include <iostream>
#include <iomanip>
#include <array>
#include <cctype>

#include "boards/boardbuilder.hpp"
#include "boards/chess.hpp"

int main() {
	auto board = getBoard(BoardType::Chess);

	auto boardState = BoardState{ 8, 8, {} };
	boardState.squares.resize(8);
	for (auto& file : boardState.squares)
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

	boardState.squares[1] = boardState.squares[6] = {
		{ PieceType::Pawn,	Color::Black },
		{ PieceType::Pawn,	Color::Black },
		{ PieceType::Pawn,	Color::Black },
		{ PieceType::Pawn,	Color::Black },
		{ PieceType::Pawn,	Color::Black },
		{ PieceType::Pawn,	Color::Black },
		{ PieceType::Pawn,	Color::Black },
		{ PieceType::Pawn,	Color::Black }
	};
	boardState.squares[0] = boardState.squares[7] = {
		{ PieceType::Rook,		Color::Black },
		{ PieceType::Knight,	Color::Black },
		{ PieceType::Bishop,	Color::Black },
		{ PieceType::Queen,		Color::Black },
		{ PieceType::King,		Color::Black },
		{ PieceType::Bishop,	Color::Black },
		{ PieceType::Knight,	Color::Black },
		{ PieceType::Rook,		Color::Black }
	};
	for (auto& squares : boardState.squares[0])	squares.piece.color = Color::White;
	for (auto& squares : boardState.squares[1])	squares.piece.color = Color::White;

	auto printBoard = [](const auto & board) {
		for (int i = board.size() - 1; i >= 0; --i) {
			auto& row = board[i];
			for (auto& col : row) {
				std::cout << ' ' << col << ' ';
			}
			std::cout << "\n\n";
		}
	};

	boardState.squares[3][2] = { {PieceType::Pawn, Color::Black}, {0, 0} };

	board->initialize(boardState);
	board->getPiece({ 1, 1 })->move({ 3, 1 }, board->getState());

	std::array<std::array<char, 8>, 8> graphicBoard = { ' ' };
	for (int rank = boardState.height - 1; rank >= 0; rank--) {
		for (int file = 0; file < boardState.width; file++) {
			auto square = board->getState().squares[rank][file];
			auto c = output(square.piece.type);
			if (square.piece.color == Color::Black)
				c = std::tolower(c);
			graphicBoard[rank][file] = c;
		}
	}

	for (auto& square : board->getPiece({ 3, 2 })->getAllAvailableMoves(board->getState())) {
		graphicBoard[square.first][square.second] = '*';
	}
	
	printBoard(graphicBoard);

	return 0;
}