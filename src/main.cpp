#include "pieces/piecebuilder.hpp"
#include "boards/boardbuilder.hpp"

#include <iostream>
#include <iomanip>
#include <array>
#include <cctype>


int main() {
	auto board = getBoard(BoardType::Chess);
	auto& boardState = board->getState();

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

	board->initialize();

	auto printBoard = [](const auto & board) {
		for (int i = board.size() - 1; i >= 0; --i) {
			auto& row = board[i];
			for (auto& col : row) {
				std::cout << ' ' << col << ' ';
			}
			std::cout << "\n\n";
		}
	};

	board->tryMove({ 1, 1 }, { 3, 1 });
	board->tryMove({ 6, 1 }, { 5, 1 });
	board->tryMove({ 1, 0 }, { 2, 0 });
	board->tryMove(PieceType::Queen, { 1, 3 });

	std::array<std::array<char, 8>, 8> graphicBoard = { ' ' };
	for (int rank = boardState.height - 1; rank >= 0; rank--) {
		for (int file = 0; file < boardState.width; file++) {
			auto& square = board->getState().squares[rank][file];
			auto c = output(square.piecePtr->getType());
			if (square.piecePtr->getColor() == Color::Black)
				c = std::tolower(c);
			graphicBoard[rank][file] = c;
		}
	}

	
	for (auto& square : board->getPiece({ 1, 3 })->getAllAvailableMoves({ 1, 3 }, board->getState())) {
		graphicBoard[square.first][square.second] = '*';
	}
	
	printBoard(graphicBoard);
	
	return 0;
}