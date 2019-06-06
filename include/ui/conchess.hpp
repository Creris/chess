#pragma once

#ifndef CONSOLE_CHESS_HEADER_H_
#define CONSOLE_CHESS_HEADER_H_

#include "../boards/chess.hpp"
#include "../boards/boardbuilder.hpp"
#include "concommand.hpp"

#include <memory>
#include <utility>
#include <string>
#include <vector>
#include <memory>

using namespace std::string_literals;

class ConsoleChess {
	std::shared_ptr<GenericBoard> board = ::getBoard(BoardType::Chess);
	int gridSize = 2;
	std::pair<int, int> turns = { 1, 1 };
protected:
	ConsoleChess() {
		initialize();
	}
	virtual ~ConsoleChess() = default;
public:
	void initialize();
	void help() const;
	void help(const std::string& command) const;

	bool promptInput();
	void render() const;
	bool isActive() const;

	Color getWinner() const;

	int grid() const;
	bool grid(int newSize);

	std::pair<int, int> turn() const;
	void turn(int b, int e);

	static ConsoleChess& getInstance();
	GenericBoard& getBoard();
};

std::string getListOfCommands();
void listCommands();

#endif // CONSOLE_CHESS_HEADER_H_
