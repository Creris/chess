
#include "movelistener.hpp"
#include "movepropagator.hpp"

#include "pieces/generic.hpp"
#include "pieces/rook.hpp"
#include "pieces/bishop.hpp"
#include "pieces/king.hpp"
#include "pieces/knight.hpp"
#include "pieces/queen.hpp"
#include "pieces/pawn.hpp"

#include <iostream>
#include <iomanip>
#include <array>
#include <cctype>

int main() {
	auto queen = std::make_shared<PiecePawn>(PiecePawn{ Color::White });
	queen->setPosition({ 2, 2 });
	auto board = BoardState{ 8, 8, {} };
	board.squares.resize(8);
	for (auto& file : board.squares)
		file.resize(8, PieceInfo{ PieceType::None, Color::None });

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
	for (auto& squares : board.squares[0])	squares.color = Color::White;
	for (auto& squares : board.squares[1])	squares.color = Color::White;

	for (auto& squares : board.squares[3]) {
		squares.color = Color::Black;
		squares.type = PieceType::Rook;
	}

	board.squares[3][2] = { PieceType::None, Color::None };

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

	std::array<std::array<char, 8>, 8> graphicBoard = { ' ' };
	for (int rank = board.height - 1; rank >= 0; rank--) {
		for (int file = 0; file < board.width; file++) {
			auto piece = board.squares[rank][file];
			auto c = output(piece.type);
			if (piece.color == Color::Black)
				c = std::tolower(c);
			graphicBoard[rank][file] = c;
		}
	}

	for (auto& square : queen->getAllAvailableMoves(board)) {
		graphicBoard[square.first][square.second] = '*';
	}
	
	printBoard(graphicBoard);

	return 0;
	
	for (int j = board.height - 1; j >= 0; --j) {
		for (int i = 0; i < board.width; i++) {
			auto piece = board.squares[i][j];
			auto c = output(piece.type);
			if (piece.color == Color::Black)
				c = std::tolower(c);
			std::cout << ' ' << c << ' ';
			if (queen->canMove({ i, j }, board))
				std::cout << " * ";
		}
		std::cout << "\n\n";
	}
}