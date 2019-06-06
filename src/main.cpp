#include "../include/pieces/piecebuilder.hpp"
#include "../include/boards/boardbuilder.hpp"
#include "../include/ui/conchess.hpp"

#include "../include/profiler.hpp"

#include <iostream>
#include <iomanip>
#include <array>
#include <cctype>
#include <string>

#include <random>

#include "../include/ai/tree.hpp"

int main()
{
	ProfileDeclare;

	ConsoleChess& chess = ConsoleChess::getInstance();
	

	///*
	auto& board = chess.getBoard();
	
	auto now = chrono::high_resolution_clock::now().time_since_epoch();

	Profiler::target("random_exec.txt");
	Profiler::traceback(0);
	Profiler::verbosity(0);

	std::random_device rd;
	std::mt19937 gen(rd());
	using Dist = std::uniform_int_distribution<int>;

	static Color colors[] = { Color::White, Color::Black };

	//std::vector<Position> _unoptimize;
	//_unoptimize.reserve(3'000'000);

	for (size_t testcase = 0; testcase < 3'000'000; testcase++) {
		auto& pieces = board.getPieces(colors[testcase % 2]);
		auto& piecePos = pieces[Dist{ 0, static_cast<int>(pieces.size()) - 1 }(gen)];

		auto allFiltered = board.getPiece(piecePos)->getAllAvailableMoves(piecePos, board.getState());
		//auto allAvail = board.getPiece(pieces[0])->getAllAvailableMoves(piecePos, board.getState());

		if (allFiltered.size())
			allFiltered[Dist{ 0, static_cast<int>(allFiltered.size()) - 1 }(gen)];
	}

	Profiler::target("");


	now = chrono::high_resolution_clock::now().time_since_epoch() - now;
	std::cout << "Total exec time: " << chrono::duration_cast<chrono::milliseconds>(now).count() << "ms.\n";

	//std::cout << (int)_unoptimize[Dist{ 0, static_cast<int>(_unoptimize.size()) }(gen)].first << "\n";
	exit(0);
	//*/

	while (1) {
		chess.render();
		std::cout << "\n";
		while (!chess.promptInput())	continue;
	}
	
	return 0;
}