#include "../include/pieces/piecebuilder.hpp"
#include "../include/boards/boardbuilder.hpp"
#include "../include/ui/conchess.hpp"

#include "../include/profiler.hpp"

#include <iostream>
#include <iomanip>
#include <array>
#include <cctype>
#include <string>

int main()
{
	ProfileDeclare;
	std::cout.sync_with_stdio(false);

	///*
	ConsoleChess& chess = ConsoleChess::getInstance();
	
	while (1) {
		chess.render();
		std::cout << "\n";
		while (!chess.promptInput())	continue;
	}
	
	return 0;
}