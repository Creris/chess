#include "../../include/ui/conchess.hpp"

#include <string>
#include <vector>
#include <array>
#include <iostream>
#include <fstream>
#include <string_view>
#include <unordered_map>
#include <iomanip>

#include "../../include/pieces/generic.hpp"
#include "../../include/profiler.hpp"
#include "../../include/stringutil.hpp"
#include "../../include/profiler.hpp"

#include "../../include/ui/concommand.hpp"
#include "../../include/ui/conactions.hpp"

using namespace std::string_literals;
using namespace std::string_view_literals;

void ConsoleChess::initialize()
{
	ProfileDeclare;
	board->initialize();
	auto turnN = board->getTurn();
	turn(std::max(1, turnN - 8), turnN);
}

inline std::string getHelpForCommand(std::string_view commandInput) {
	ProfileDeclare;
	auto split_ = split(commandInput, " \t\n");
	if (split_.size() > 0 && (split_[0] == "To" || split_[0] == "From")) {
		split_.erase(split_.begin());
		return strip(getHelpForCommand("move") + "\n"
					 + getHelpForCommand(join(split_, " ")));
	}

	if (split_.size() != 1) {
		std::string msg;
		for (auto& cmd : split_) {
			msg += getHelpForCommand(cmd) + "\n\n";
		}
		return strip(msg);
	}

	auto command = split_[0];
	auto cmd = cmdToIdx(command);
	auto pos = commandHelpStrings.find(cmd);
	if (pos == commandHelpStrings.end())
		return "Unkown command "s + std::string{ command };

	auto& pair = pos->second;

	auto spacesPrefix = "                  "s;
	auto joined = join(split(std::string_view{ pair.second }, "\n"), "\n" + spacesPrefix);
	auto stripped = strip(joined);

	return strip(pair.first) + "\n" +
		spacesPrefix + stripped;
}

void ConsoleChess::help() const
{
	ProfileDeclare;
	std::vector<std::string> helps;

	for (auto& v : commandHelpStrings) {
		helps.emplace_back(split(v.second.first)[0]);
	}
	
	std::sort(helps.begin(), helps.end(), [](const auto& lhs, const auto& rhs) {
		const auto result = std::mismatch(lhs.cbegin(), lhs.cend(),
									 rhs.cbegin(), rhs.cend(), [](auto lhs, auto rhs) {
			return std::tolower(lhs) == std::tolower(rhs);
		});

		return result.second != rhs.cend()
			&& (result.first == lhs.cend()
				|| std::tolower(*result.first) < std::tolower(*result.second)
			);
	});

	std::cout << "Help for commands:\n\n";

	for (auto& v : helps) {
		std::cout << getHelpForCommand(v) << "\n\n";
	}

	return;
}
void ConsoleChess::help(const std::string& command) const
{
	ProfileDeclare;
	if (command == "") {
		help();
		return;
	}

	auto helpStr = getHelpForCommand(command);

	if (helpStr == "") {
		help();
		return;
	}

	std::cout << "Help for command " << command << ":\n\n";
	std::cout << helpStr << "\n\n";
}

std::string getListOfCommands() {
	ProfileDeclare;
	std::vector<std::pair<std::string, std::string>> commands;
	size_t maxLength = 0;

	for (auto& v : commandHelpStrings) {
		auto cmds = split(v.second.first, "\n");
		for (auto& cmd : cmds) {
			auto words = split(cmd, " ", 1);
			maxLength = std::max(maxLength, words[0].size());

			if (words.size() > 1)
				commands.emplace_back(words[0], words[1]);
			else
				commands.emplace_back(words[0], "");
		}
	}

	maxLength++;

	std::sort(commands.begin(), commands.end(), [](const auto& lhs, const auto& rhs) {
		const auto result = std::mismatch(lhs.first.cbegin(), lhs.first.cend(),
									 rhs.first.cbegin(), rhs.first.cend(), [](auto lhs, auto rhs) {
			return std::tolower(lhs) == std::tolower(rhs);
		});

		return result.second != rhs.first.cend()
			&& (result.first == lhs.first.cend()
				|| std::tolower(*result.first) < std::tolower(*result.second)
			);
	});

	std::string cmds;

	for (auto& cmd : commands) {
		std::string line;
		line.resize(maxLength, ' ');
		for (size_t i = 0; i < cmd.first.size(); ++i)
			line[i] = cmd.first[i];
		cmds += line + cmd.second + "\n";
	}

	return cmds;
}

void listCommands() {
	ProfileDeclare;
	std::cout << "List of commands:\n\n";
	std::cout << "  " << join(split(getListOfCommands(), "\n"), "\n  ") << "\n\n";
	std::cout << "For more information about each command, please write help CMD\n";
	std::cout << "or just regular help.\n\n";
}

inline std::vector<std::string_view> _strsToStreams(const std::vector<std::string>& strs) {
	ProfileDeclare;
	std::vector<std::string_view> v;
	for (auto& str : strs)	v.emplace_back(str);
	return v;
}

inline bool handleCommand(std::string_view cmd, const std::vector<std::string>& args) {
	ProfileDeclare;
	auto cmdV = cmdToIdx(cmd);
	if (cmdV == Command::Invalid) {
		ConsoleChess::getInstance().help();
		return false;
	}
	return commandHandlers[cmdV](ConsoleChess::getInstance().getBoard(), _strsToStreams(args));
}

bool ConsoleChess::promptInput()
{
	ProfileDeclare;
	std::string userInput;
	std::cout << "User input: ";
	std::getline(std::cin, userInput);
	std::cout << "\n";

	auto split_ = split(strip(userInput), " \t");
	if (!split_.size())	return false;
	auto cmd = split_[0];
	auto pos = stringToPosition(cmd);
	if (pos.first != -1 && pos.second != -1)
		return handleCommand("move", split_);

	split_.erase(split_.begin());

	return handleCommand(cmd, split_);
}

Color ConsoleChess::getWinner() const
{
	return board->getWinner();
}

int ConsoleChess::grid() const
{
	return gridSize;
}

bool ConsoleChess::grid(int newSize)
{
	if (newSize != 1 && newSize != 2 && newSize != 4 && newSize != 8)	return false;
	gridSize = newSize;
	return true;
}

std::pair<int, int> ConsoleChess::turn() const
{
	return turns;
}

void ConsoleChess::turn(int b, int e)
{
	turns = { b, e };
}

ConsoleChess& ConsoleChess::getInstance()
{
	static ConsoleChess c;
	return c;
}

GenericBoard& ConsoleChess::getBoard()
{
	return *getInstance().board;
}

void ConsoleChess::render() const
{
	ProfileDeclare;

	static auto _generateEmpty = [](const auto& board, 
									const std::array<std::string, 9>& grid,
									int gridSize) {
		ProfileDeclare;
		std::string out = "  A   B   C   D   E   F   G   H  \n";
		for (int i = board.size() - 1; i >= 0; --i) {
			if ((i % gridSize) == (gridSize - 1))
				out += "+---+---+---+---+---+---+---+---+\n";
			else
				out += grid[gridSize] + "\n";

			for (size_t j = 0; j < board[i].size(); ++j) {
				if (!(j % gridSize))
					out += "|   ";
				else
					out += "    ";
			}
			out += "|\n";
		}

		out += "+---+---+---+---+---+---+---+---+\n";
		out += "  A   B   C   D   E   F   G   H  \n";

		return out;
	};

	static auto printBoard = [&](const auto& board, int gridSize = 2) {
		ProfileDeclare;
		if (gridSize < 1 || gridSize > 8)
			return;

		static std::array<std::string, 9> grid = {
			""s,
			"+   +   +   +   +   +   +   +   +"s,
			"+       +       +       +       +"s,
			""s,
			"+               +               +"s,
			""s,
			""s,
			""s,
			"+                               +"s
		};

		std::cout << "\n";

		auto& realBoard = ConsoleChess::getInstance().getBoard();

		auto selected = realBoard.getSelectedPosition();
		std::string output = _generateEmpty(board, grid, gridSize);
		std::vector<std::string> arrOut = split(output, "\n");

		for (size_t i = 0; i < arrOut.size(); ++i) {
			arrOut[i] = "   "s + arrOut[i];
			arrOut[i].resize(38, ' ');
		}

		for (size_t i = 2; i < arrOut.size() - 1; i += 2) {
			auto num = static_cast<char>(8 - i / 2 + 1);
			arrOut[i][1] = '0' + num;
			arrOut[i][37] = '0' + num;
		}

		static auto computePosition = [](Position boardPos) {
			return Position{ 2 + 2 * boardPos.first, 5 + 4 * boardPos.second };
		};

		auto& boardState = realBoard.getState();
		for (int i = 0; i < boardState.height; ++i) {
			for (int j = 0; j < boardState.width; ++j) {
				auto piece = realBoard.getPiece({ i, j });
				if (!piece)	continue;
				auto pieceChar = typeToChar(piece->getType());
				if (piece->getColor() == Color::Black)
					pieceChar = std::tolower(pieceChar);

				auto pos = computePosition({ i, j });
				arrOut[arrOut.size() - 1 - pos.first][pos.second] = pieceChar;
			}
		}


		if (selected.first != -1 || selected.second != -1) {
			auto selPos = computePosition(selected);
			arrOut[arrOut.size() - 1 - selPos.first - 1][selPos.second] = 'v';
			arrOut[arrOut.size() - 1 - selPos.first][selPos.second - 1] = '>';
			arrOut[arrOut.size() - 1 - selPos.first][selPos.second + 1] = '<';
			arrOut[arrOut.size() - 1 - selPos.first + 1][selPos.second] = '^';

			for (auto& move : realBoard.getPossibleMoves()) {
				auto movePos = computePosition(move);
				auto& chr = arrOut[arrOut.size() - 1 - movePos.first][movePos.second];
				if (chr != ' ') {
					arrOut[arrOut.size() - 1 - movePos.first - 1][movePos.second] = '~';
					arrOut[arrOut.size() - 1 - movePos.first][movePos.second - 1] = '!';
					arrOut[arrOut.size() - 1 - movePos.first][movePos.second + 1] = '!';
					arrOut[arrOut.size() - 1 - movePos.first + 1][movePos.second] = '~';
				}
				else {
					arrOut[arrOut.size() - 1 - movePos.first][movePos.second] = '*';
				}
			}
		}
		
		if (realBoard.getWinner() == Color::Pat) {
			std::cout << "Game over! The game ended in Pat.\n\n";
		}
		else if (realBoard.getWinner() != Color::None) {

			std::cout << "Game over! Winner is " << colorToName(realBoard.getWinner()) << "\n\n";
		}
		else {
			std::cout << "Move: " << realBoard.getTurn() << ", Turn: "
				<< colorToName(realBoard.getPlayingColor()) << "\n\n";
		}

		auto turns = ConsoleChess::getInstance().turn();

		if (turns.second - turns.first > 8)
			turns.second = turns.first + 8;

		auto movesWidth = 21;

		std::vector<std::string> turnTable;
		turnTable.emplace_back("       Moves:");
		turnTable.emplace_back();

		for (auto& v : realBoard.getTurnInfo(turns.first, turns.second)) {
			auto turnStr = std::to_string(turns.first);
			if (turns.first < 10)
				turnStr = "  " + turnStr;
			else if (turns.first < 100)
				turnStr = " " + turnStr;

			turnTable.emplace_back(turnStr + ". " + v.first + " " + v.second);
			turnTable.emplace_back();
			turns.first++;
		}

		turnTable.resize(arrOut.size(), "");
		for (auto& v : turnTable)	v.resize(movesWidth, ' ');

		static auto _formatGrave = [&](GenericBoard& board, Color c, int width) {
			ProfileDeclare;

			std::vector<std::string> formatted;
			formatted.emplace_back();
			int counter = 0;
			for (auto& ps : board.getGraveyard(c)) {
				formatted.back() += typeToChar(ps.piecePtr->getType()) + ", "s;
				if (counter++ == 3) {
					formatted.emplace_back();
					counter = 0;
				}
			}

			if (!formatted.back().size() && formatted.size())
				formatted.pop_back();

			if (formatted.size())
				formatted.back() = strip(formatted.back(), ", ");

			return formatted;
		};

		std::vector<std::string> graveyard;
		graveyard.emplace_back(" Graveyards:");
		graveyard.emplace_back("");
		graveyard.emplace_back("   White:");
		graveyard.emplace_back("");

		std::vector<std::string> wGraveFormatted = _formatGrave(realBoard, Color::White, 14);
		std::vector<std::string> bGraveFormatted = _formatGrave(realBoard, Color::Black, 14);
		graveyard.insert(graveyard.end(), wGraveFormatted.begin(), wGraveFormatted.end());

		graveyard.resize(9);
		graveyard.emplace_back("");
		graveyard.emplace_back("   Black:");
		graveyard.emplace_back("");
		graveyard.insert(graveyard.end(), bGraveFormatted.begin(), bGraveFormatted.end());

		graveyard.resize(arrOut.size(), "");
		for (auto& v : graveyard)	v.resize(14, ' ');

		output = interleave(arrOut, turnTable, " | ");
		output = strip(interleave(split(output, "\n"), graveyard, " | "));

		printf("     %s\n", output.c_str());
	};

	auto& boardState = board->getState();

	std::array<std::array<char, 8>, 8> graphicBoard = { ' ' };
	for (int rank = boardState.height - 1; rank >= 0; rank--) {
		for (int file = 0; file < boardState.width; file++) {
			auto& square = board->getState().squares[rank][file];
			auto c = typeToChar(square.piecePtr->getType());
			if (square.piecePtr->getColor() == Color::Black)
				c = std::tolower(c);
			graphicBoard[rank][file] = c;
		}
	}

	for (auto& move : board->getPossibleMoves()) {
		graphicBoard[move.first][move.second] = '*';
	}

	printBoard(graphicBoard, gridSize);

	actions::turn(*board, { "reset" });
}

bool ConsoleChess::isActive() const
{
	return board->getWinner() != Color::None;
}
