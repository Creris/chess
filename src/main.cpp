#include "pieces/piecebuilder.hpp"
#include "boards/boardbuilder.hpp"

#include "ui/conchess.hpp"

#include <iostream>
#include <iomanip>
#include <array>
#include <cctype>

#include <string>

class S {
public:
	~S() {
		std::cout << "~S()\n";
	}
}s;

int main()
{
	exit(0);
	///*
	ConsoleChess& chess = ConsoleChess::getInstance();
	
	while (1) {
		chess.render();
		std::cout << "\n";
		while (!chess.promptInput());
	}
	//*/
	
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

	auto printBoard = [](const auto & board, int gridSize = 2) {
		if (gridSize < 1 || gridSize > 8)
			return;

		static std::string grid[] = {
			"",
			"+   |   |   |   |   |   |   |   +",
			"+       |       |       |       +",
			"",
			"+               |               +",
			"",
			"",
			"",
			"+                               +"
		};

		for (int i = board.size() - 1; i >= 0; --i) {
			auto& row = board[i];

			if (i % gridSize == (gridSize - 1))
				std::cout << "+---+---+---+---+---+---+---+---+\n";
			else {
				auto& grd = grid[gridSize];
				std::cout << grd << "\n";
			}

			for (size_t j = 0; j < row.size(); ++j) {
				if (!(j % gridSize))
					std::cout << "| " << row[j] << " ";
				else
					std::cout << "  " << row[j] << " ";
			}
			std::cout << "|\n";
		}
		std::cout << "+---+---+---+---+---+---+---+---+\n";
	};

	/*
	board->tryMove({ 1, 3 }, { 1, 4 });
	board->tryMove({ 4, 4 }, { 1, 4 });
	board->tryMove({ 0, 4 }, { 1, 4 });
	*/

	std::cout << "White graveyard: \n";
	for (auto& gv : board->getGraveyard(Color::White))
		std::cout << "  " << output(gv.piecePtr->getType()) << "\n";

	std::cout << "Black graveyard: \n";
	for (auto& gv : board->getGraveyard(Color::Black))
		std::cout << "  " << output(gv.piecePtr->getType()) << "\n";

	/*
	board->tryMove({ 1, 1 }, { 3, 1 });
	board->tryMove({ 7, 1 }, { 5, 0 });
	*/

	srand(time(nullptr));

	auto upgrader = [](PieceType type, const std::vector<PieceType>& choices) {
		return choices[rand() % choices.size()];
	};

	board->tryMove({ 5, 0 }, { 6, 0 }, upgrader);
	board->tryMove({ 1, 6 }, { 0, 6 }, upgrader);
	board->tryMove({ 6, 0 }, { 7, 0 }, upgrader);


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
	
	for (auto& square : board->getKing(Color::White).piecePtr->getAllAvailableMoves({ 1, 4 }, board->getState())) {
		graphicBoard[square.first][square.second] = '*';
	}
	
	printBoard(graphicBoard, 1);
	
	return 0;
}