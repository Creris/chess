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

using namespace std::string_literals;
using namespace std::string_view_literals;

inline auto charToType = [](char c) {
	switch (std::tolower(c)) {
		case 'p':
			return PieceType::Pawn;
		case 'b':
			return PieceType::Bishop;
		case 'k':
			return PieceType::King;
		case 'n':
			return PieceType::Knight;
		case 'q':
			return PieceType::Queen;
		case 'r':
			return PieceType::Rook;
		default:
			return PieceType::None;
	}
};

inline auto typeToChar = [](PieceType type) {
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
			return ' ';
	}
	return ' ';
};

std::shared_ptr<GenericBoard>& getChessBoard()
{
	return ConsoleChess::getInstance().board;
}

void ConsoleChess::initialize()
{
	ProfileDeclare;
	board->initialize();
	auto turnN = board->getTurn();
	turn(std::max(1, turnN - 8), turnN);
}

enum class Command {
	Help,
	Commands,
	Select,
	Unselect,
	Info,
	Restart,
	Forfeit,
	Move,
	Turn,
	Quit,
	Exit,
	Grid,
	Render,
	Profile,
	Export,

	Invalid
};

inline std::unordered_map<Command, std::pair<std::string, std::string>> commandHelpStrings = {
	{ Command::Quit, std::make_pair("quit"s,
			"Quits the chess game."s)
	},
	{ Command::Exit, std::make_pair("exit"s,
			"Quits the chess game."s)
	},
	{ Command::Help, std::make_pair("help"s,
			"Prints this help message."s)
	},
	{ Command::Commands, std::make_pair("commands"s,
			"Prints list of available commands with no\n"s
			" additional information."s)
	},
	{ Command::Select, std::make_pair("select POSITION"s,
			"Selects a piece at this position to \n"s
			"use actions with.\n"s
			"Can only select a piece owner by the player\n"s
			"that is currently playing.\n"s
			"Valid Positions: [a..h/A..H][1..8].")
	},

	{ Command::Unselect, std::make_pair("unselect"s,
			"Unselects a piece if any was selected."s)
	},
	{ Command::Info, std::make_pair("info POSITION"s,
			"Displays information about a piece on given position.\n"s
			"use actions with.\n"s
			"Can only select a piece owner by the player\n"s
			"that is currently playing.")
	},
	{ Command::Restart, std::make_pair("restart"s,
			"Restarts the chess game moving all pieces\n"s
			"to their initial positions."s)
	},
	{ Command::Forfeit,	std::make_pair("forfeit"s,
			"Forfeits the match, in case the player is checked.\n"s
			"Concludes the winner of the match the opposing player."s)
	},
	{ Command::Move, std::make_pair("move To\nmove From To"s,
			"Perform a move from position From into position To.\n"s
			"If only one coordinate is provided, attempts to move the\n"s
			"selected piece.\n"s
			"If both coordinates are provided, has same effect as\n"s
			"select From, move To.")
	},
	{ Command::Turn, std::make_pair("turn V\nturn V U\nturn reset"s,
			"Displays information about turn number V.\n"
			"If U is provided, displays turns from turn V until turn U.\n"
			"turn reset resets the turn display information to default."s)
	},
	{ Command::Grid, std::make_pair("grid N"s,
			"Sets the grid size for the chess. Available values:\n"s
			"1, 2, 4, 8(default)."s)
	},
	{ Command::Render, std::make_pair("render"s,
			"Renders the chess board."s)
	},
	{ Command::Profile, std::make_pair("profile off\nprofile [on] file\n"s
									   "profile [on] file verbose\nprofile [on] file traceback\n"s
									   "profile [on] file total"s,
			"Turns profiling of the program on or off.\n"s
			"Profiling puts the execution time of every"s "\n"s
			"function into a file.\n"s
			"If verbose is provided, will also include stack trace.\n"s
			"Note that total profiling uses a lot of disk space.")
	},
	{ Command::Export, std::make_pair("export FILE"s,
			"Exports the list of moves made up until this point into\n"s
			"a file."s)
	}
};

inline auto cmdToIdx = [](std::string_view input) {
	ProfileDeclare;
	static std::unordered_map<std::string_view, Command> map = {
		{ "quit", Command::Quit },
		{ "exit", Command::Exit },
		{ "help", Command::Help },
		{ "commands", Command::Commands },
		{ "select", Command::Select },
		{ "unselect", Command::Unselect },
		{ "info", Command::Info },
		{ "restart", Command::Restart },
		{ "forfeit", Command::Forfeit },
		{ "move", Command::Move },
		{ "turn", Command::Turn },
		{ "grid", Command::Grid },
		{ "render", Command::Render },
		{ "profile", Command::Profile },
		{ "export", Command::Export }
	};

	if (!map.contains(input))	return Command::Invalid;
	return map.at(input);
};

inline std::string getHelpForCommand(std::string_view commandInput) {
	ProfileDeclare;
	auto split_ = split(commandInput, " \t\n");
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
	if (pos == commandHelpStrings.end())	return "";
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
		helps.emplace_back(split(v.second.first, " ")[0]);
	}

	std::sort(helps.begin(), helps.end(), [](const auto& v1, const auto& v2) {
		return v1 < v2;
	});

	std::cout << "Help for commands:\n";

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

	std::cout << "\nHelp for command " << command << ":\n";
	std::cout << helpStr << "\n\n";
}

std::string getListOfCommands() {
	ProfileDeclare;
	std::vector<std::pair<std::string, std::string>> helps;

	size_t maxLength = 0;

	for (auto& v : commandHelpStrings) {
		auto splt = split(v.second.first, " ", 1);
		if (splt.size() == 1)
			helps.emplace_back(splt[0], "");
		else
			helps.emplace_back(splt[0], splt[1]);
		maxLength = std::max(helps.back().first.size(), maxLength);
	}

	maxLength++;

	std::sort(helps.begin(), helps.end(), [](const auto& v1, const auto& v2) {
		return v1.first < v2.first;
	});

	std::string cmds;

	for (auto& storage : helps) {
		std::string line;
		line.resize(maxLength, ' ');
		for (size_t i = 0; i < storage.first.size(); ++i)
			line[i] = storage.first[i];
		cmds += line + storage.second + "\n";
	}

	return strip(cmds);
}

void listCommands() {
	ProfileDeclare;
	std::cout << "List of commands:\n";
	std::cout << "  Command - Arguments\n";
	std::cout << "  " << join(split(getListOfCommands(), "\n"), "\n  ") << "\n\n";
}

using CommandHandler = bool(*)(const std::vector<std::string_view>& args);

namespace actions {
	bool help(const std::vector<std::string_view>& args) {
		ProfileDeclare;
		ConsoleChess::getInstance().help(join(args, " "));
		return false;
	}
	
	bool _internalHelp(const std::vector<std::string_view>& args) {
		ProfileDeclare;
		std::cout << "Invalid arguments for command " << args[0] << ".\n";
		ConsoleChess::getInstance().help(join(args, ""));
		return true;
	}

	bool export_moves(const std::vector<std::string_view>& args) {
		ProfileDeclare;
		if (args.size() != 1)	return _internalHelp({ "export" });
		std::ofstream moves{ std::string{ args[0] }, std::ios_base::binary };
		auto& board = getChessBoard();
		auto allTurns = board->getTurnInfo(1, board->getTurn());
		int counter = 1;
		std::string result;
		for (auto& v : allTurns) {
			result += std::to_string(counter++) + ". " + v.first + " " + v.second + "\n";
		}

		result.shrink_to_fit();
		if (moves.write(&result[0], result.size()))
			std::cout << "Wrote moves up until now into file " << args[0] << "\n\n";
		return false;
	}

	bool commands(const std::vector<std::string_view>& args) {
		ProfileDeclare;
		if (args.size())	return _internalHelp({ "commands" });
		listCommands();
		return false;
	}

	bool quit(const std::vector<std::string_view>& args) {
		ProfileDeclare;
		if (args.size())	return _internalHelp({ "quit" });
		std::cout << "Quiting chess.\n";
		exit(0);
	}

	bool grid(const std::vector<std::string_view>& args) {
		ProfileDeclare;
		if (args.size() != 1)	return _internalHelp({ "grid" });
		int size = -1;
		try {
			size = std::stoi(std::string{ args[0] });
		} catch (std::invalid_argument&) {
			return _internalHelp({ "grid" });
		} catch (std::out_of_range&) {
			return _internalHelp({ "grid" });
		} catch (...) {
			std::cout << "Unexpected exception while handling grid rescaling.\n\n";
			exit(0);
		}
		if (!ConsoleChess::getInstance().grid(size))
			return _internalHelp({ "grid" });

		return true;
	}

	bool render(const std::vector<std::string_view>& args) {
		ProfileDeclare;
		if (args.size())	return _internalHelp({ "render" });
		ConsoleChess::getInstance().render();
		std::cout << "\n";
		return false;
	}

	bool turn(const std::vector<std::string_view>& args) {
		ProfileDeclare;
		if (args.size() > 2 || !args.size())	return _internalHelp({ "turn" });
		if (args[0] == "reset") {
			auto current = getChessBoard()->getTurn();
			ConsoleChess::getInstance().turn(std::max(current - 8, 1), current);
			return true;
		}
		
		int turnU = 0;
		int turnV = -1;
		bool turnVIn = false;
		int maxTurn = getChessBoard()->getTurn();

		try {
			turnU = std::stoi(std::string{ args[0] });
			if (args.size() == 2) {
				turnV = std::stoi(std::string{ args[1] });
				turnVIn = true;
			}
		} catch (...) {
			return _internalHelp({ "turn" });
		}

		if (turnU > maxTurn || turnU < 1)
			return _internalHelp({ "turn" });

		if (!turnVIn) {

			auto info = getChessBoard()->getTurnInfo(turnU);
			std::cout << "  " << turnU << ". " << info.first << "  " << info.second << "\n\n";
			return false;
		}

		if (turnV > maxTurn || turnV < turnU)
			return _internalHelp({ "turn" });

		ConsoleChess::getInstance().turn(turnU, turnV);

		return true;
	}

	bool restart(const std::vector<std::string_view>& args) {
		ProfileDeclare;
		if (args.size())	return _internalHelp({ "restart" });
		ConsoleChess::getInstance().initialize();
		return true;
	}

	bool select(const std::vector<std::string_view>& args) {
		ProfileDeclare;
		if (args.size() != 1) {
			return _internalHelp({ "select" });
		}
		Position pos = stringToPosition(args[0]);
		if (pos.first == -1 || pos.second == -1) {
			return _internalHelp({ "select" });
		}
		auto& board = getChessBoard();
		if (!board->select(pos)) {
			std::cout << "Can't select piece at " << args[0] << ".";
			auto piece = board->getPiece(pos);
			if (!piece) {
				std::cout << " No piece found at this position.\n";
			}
			else if (piece->getType() == PieceType::ShadowPawn || 
					 piece->getType() == PieceType::None) {
				std::cout << " No piece found at this position.\n";
			}
			else if (piece->getColor() != board->getPlayingColor()) {
				std::cout << " Piece is not owned by the playing player.\n";
			}
			else {
				std::cout << " Unknown reason.\n";
			}
			std::cout << "\n";
		}

		return true;
	}

	bool unselect(const std::vector<std::string_view>& args) {
		ProfileDeclare;
		if (args.size())	return _internalHelp({ "unselect" });
		getChessBoard()->unselect();
		return true;
	}


	static auto typeToName = [](PieceType type) {
		switch (type) {
			case PieceType::Bishop:
				return "Bishop [B]"s;
			case PieceType::King:
				return "King [K]"s;
			case PieceType::Knight:
				return "Knight [N]"s;
			case PieceType::None:
			case PieceType::ShadowPawn:
				return "No piece"s;
			case PieceType::Pawn:
				return "Pawn [P]"s;
			case PieceType::Queen:
				return "Queen [Q]"s;
			case PieceType::Rook:
				return "Rook [R]"s;
			default:
				return ""s;
		}
	};

	static auto colorToName = [](Color c) {
		switch (c) {
			case Color::White:
				return "White"s;
			case Color::Black:
				return "Black"s;
			case Color::Pat:
				return "Pat"s;
			default:
				return ""s;
		}
	};

	static auto pieceToName = [](PieceType type, Color c) {
		ProfileDeclare;
		auto typeName = typeToName(type);
		auto colorName = colorToName(c);

		if (c == Color::Black && typeName.size() && typeName.back() == ']')
			typeName[typeName.size() - 2] = std::tolower(typeName[typeName.size() - 2]);

		return typeName;
	};

	std::vector<Position> _getSelfCheckPositions(Position atPosition,
												 const std::vector<Position>& valid) {
		ProfileDeclare;
		auto& board = getChessBoard();
		auto storage = board->getPieceStorage(atPosition);
		if (!storage.piecePtr)	return {};
		if (storage.piecePtr->getType() == PieceType::ShadowPawn ||
			storage.piecePtr->getType() == PieceType::None)
			return {};

		auto allMoves = storage.piecePtr->getAllAvailableMoves(atPosition, board->getState());
		allMoves.erase(std::remove_if(allMoves.begin(), allMoves.end(), [&](const Position& p) {
			return std::find(valid.begin(), valid.end(), p) != valid.end();
		}), allMoves.end());

		return allMoves;
	}

	void _printPossibleMoves(Position pos) {
		ProfileDeclare;
		if (pos.first == -1 || pos.second == -1)	return;
		auto& board = getChessBoard();
		auto piece = board->getPieceStorage(pos);

		auto valid = board->getPossibleMoves(pos);
		auto invalid = _getSelfCheckPositions(pos, valid);

		static auto _cleanBack = [](std::string& s) {
			if (s.size() >= 2 && s.ends_with(", ")) {
				s.pop_back();
				s.pop_back();
			}
		};

		std::string output;

		if (valid.size()) {
			output += "\n    ";
			auto counter = 0;
			for (auto& move : valid) {
				output += positionToString(move);
				auto target = board->getPiece(move);
				if (target && target->getColor() != piece.piecePtr->getColor()
						&& target->getType() != PieceType::ShadowPawn
						&& target->getType() != PieceType::None)
					output += "(captures "s + pieceToName(target->getType(), target->getColor()) + ")"s;
				output += ", ";

				if (counter++ == 3) {
					counter = 0;
					output += "\n    ";
				}
			}
			_cleanBack(output);
		}
		else {
			output += "None\n";
		}

		if (invalid.size()) {
			output += "\n"s + "  Moves that would cause self-check:";
			auto counter = 0;
			output += "\n    "s;
			for (auto& move : invalid) {
				output += positionToString(move);
				auto target = board->getPiece(move);
				if (target && target->getColor() != piece.piecePtr->getColor()
						&& target->getType() != PieceType::ShadowPawn
						&& target->getType() != PieceType::None)
					output += "(captures "s + pieceToName(target->getType(), target->getColor()) + ")"s;
				output += ", ";

				if (counter++ == 3) {
					counter = 0;
					output += "\n    ";
				}
			}
			_cleanBack(output);
		}

		auto stripped = strip(output);
		if (!stripped.size())
			std::cout << "    None.\n\n";
		else
			std::cout << "    " << strip(output) << "\n\n";
	}

	bool info(const std::vector<std::string_view>& args) {
		ProfileDeclare;
		if (args.size() != 1)	return _internalHelp({ "info" });
		Position pos = stringToPosition(args[0]);
		if (pos.first == -1 || pos.second == -1)	return _internalHelp({ "info" });
		auto storage = getChessBoard()->getPieceStorage(pos);
		std::cout << "Information about position " << args[0] << ":\n";
		auto type = PieceType::None;
		auto color = Color::None;
		if (storage.piecePtr) {
			type = storage.piecePtr->getType();
			color = storage.piecePtr->getColor();
		}
		auto colorName = colorToName(color);
		auto typeName = pieceToName(type, color);

		const char* words[] = { "piece is", "pieces are" };
		int whiteAttacking = static_cast<int>(storage.threat[0]);
		int blackAttacking = static_cast<int>(storage.threat[1]);

		std::cout << "  Piece name: " << typeName;
		if (type != PieceType::None && type != PieceType::ShadowPawn)
			std::cout << ", Piece color: " << colorName;
		std::cout << "\n";

		if (type != PieceType::ShadowPawn && type != PieceType::None) {
			std::cout << "  Piece at this position has following moves available:\n";
			_printPossibleMoves(pos);
		}

		std::cout << "  Threats:    " << whiteAttacking << " White " 
										<< words[whiteAttacking != 1] << " attacking this square.\n";
		std::cout << "              " << blackAttacking << " Black "
										<< words[blackAttacking != 1] << " attacking this square.\n";

		std::cout << "\n";

		return true;
	}

	bool forfeit(const std::vector<std::string_view>& args) {
		ProfileDeclare;
		if (args.size())	return _internalHelp({ "forfeit" });
		std::cout << "Player " << colorToName(getChessBoard()->getPlayingColor()) << " forfeits!\n";
		getChessBoard()->forfeit();
		render({});

		do {
			std::cout << "(R) Restart\n";
			std::cout << "(E) Export moves\n";
			std::cout << "(Q) Quit\n";
			std::cout << "Choice: ";
			std::string input;
			std::getline(std::cin, input);
			std::cout << "\n";
			if (input.size() > 1 && std::tolower(input[0]) != 'e')	continue;
			switch (std::tolower(input[0])) {
				case 'r':
					return restart({});
				case 'q':
					return quit({});
				case 'e':
				{
					auto split_ = split(std::string_view{ input });
					split_.erase(split_.begin());
					export_moves(split_);
				}
			}
		} while (true);
		return true;
	}

	bool _moveSelected(std::string_view to) {
		ProfileDeclare;
		auto selected = getChessBoard()->getSelectedPosition();
		if (selected.first == -1 || selected.second == -1)
			return _internalHelp({ "move" });

		Position pos = stringToPosition(to);
		if (pos.first == -1 || pos.second == -1) {
			std::cout << "Invalid move position " << to << "\n\n";
			return false;
		}

		static auto promotion = [](PieceType orig, const std::vector<PieceType>& choices) {
			ProfileDeclare;
			std::string origName = split(typeToName(orig), " ")[0];
			std::vector<std::string> choiceNames;
			size_t maxLen = 0;
			for (auto& p : choices) {
				choiceNames.emplace_back(split(typeToName(p), " ")[0]);
				maxLen = std::max(maxLen, choiceNames.back().size());
			}

			std::cout << "Promoting " << origName << ". ";
			PieceType selected = PieceType::None;

			do {
				std::cout << "Select from following:\n";
				for (size_t i = 0; i < choices.size(); ++i) {
					std::cout << std::setw(maxLen) << choiceNames[i];
					if (choiceNames[i] == "Knight") {
						choiceNames[i][0] = 'N';
						std::cout << " (" << choiceNames[i][0] << ")\n";
						choiceNames[i][0] = 'K';
					}
					else {
						std::cout << " (" << choiceNames[i][0] << ")\n";
					}
				}
				std::cout << "Choice: ";
				std::string choice;
				std::getline(std::cin, choice);
				std::cin.ignore(-1);

				if (choice.size() != 1)	continue;
				selected = charToType(choice[0]);
			} while (std::find(choices.begin(), choices.end(), selected) == choices.end());

			return selected;
		};

		if (!getChessBoard()->tryMove(pos, promotion)) {
			std::cout << "This move cannot be performed. Try again.\n\n";
			return false;
		}
		unselect({});
		turn({ "reset" });
		return true;
	}

	bool _moveSelect(std::string_view from, std::string_view to) {
		ProfileDeclare;
		Position fromPos = stringToPosition(from);
		if (fromPos.first == -1 || fromPos.second == -1) {
			std::cout << "Invalid starting position " << from << "\n\n";
			return false;
		}
		getChessBoard()->select(fromPos);
		return _moveSelected(to);
	}

	bool move(const std::vector<std::string_view>& args) {
		ProfileDeclare;
		if (args.size() == 1)		return _moveSelected(args[0]);
		else if (args.size() == 2)	return _moveSelect(args[0], args[1]);
		else						return _internalHelp({ "move" });
	}

	bool profile(const std::vector<std::string_view>& args) {
		ProfileDeclare;
		auto size = args.size();
		if (!size || (size == 1 && args[0] == "off")) {
			auto oldFile = Profiler::target();
			Profiler::target("");
			if (oldFile.size()) {
				std::cout << "Wrote collected metrics into " << oldFile << "\n";
			}

			return true;
		}
		else if (size > 0 && args[0] != "off") {
			auto rest = std::vector<std::string_view>{ args.begin(), args.end() };
			if (args[0] == "on")	rest.erase(rest.begin());
			if (rest.size() > 2) {
				return _internalHelp({ "profile" });
			}
			
			if (rest.size() > 1) {
				auto back = rest.back();
				if (back == "verbose") {
					Profiler::traceback(0);
					Profiler::verbosity(1);
				}
				else if (back == "traceback") {
					Profiler::traceback(1);
					Profiler::verbosity(0);
				}
				else if (back == "total") {
					Profiler::traceback(1);
					Profiler::verbosity(1);
				}
				else return _internalHelp({ "profile" });
			}

			Profiler::target(std::string{ rest[0] });
			return true;
		}

		return _internalHelp({ "profile" });
	}
}

std::unordered_map<Command, CommandHandler> commandHandlers = {
	{ Command::Help,		actions::help },
	{ Command::Commands,	actions::commands },
	{ Command::Select,		actions::select },
	{ Command::Unselect,	actions::unselect },
	{ Command::Info,		actions::info },
	{ Command::Restart,		actions::restart },
	{ Command::Forfeit,		actions::forfeit },
	{ Command::Move,		actions::move },
	{ Command::Turn,		actions::turn },
	{ Command::Quit,		actions::quit },
	{ Command::Exit,		actions::quit },
	{ Command::Grid,		actions::grid },
	{ Command::Render,		actions::render },
	{ Command::Profile,		actions::profile },
	{ Command::Export,		actions::export_moves },
};

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
	return commandHandlers[cmdV](_strsToStreams(args));
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

		auto selected = getChessBoard()->getSelectedPosition();
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

		auto& realBoard = getChessBoard();
		auto& boardState = realBoard->getState();
		for (int i = 0; i < boardState.height; ++i) {
			for (int j = 0; j < boardState.width; ++j) {
				auto piece = realBoard->getPiece({ i, j });
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

			for (auto& move : realBoard->getPossibleMoves()) {
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
		
		if (realBoard->getWinner() != Color::None) {
			std::cout << "Game over! Winner is " << actions::colorToName(realBoard->getWinner()) << "\n\n";
		}
		else {
			std::cout << "Move: " << realBoard->getTurn() << ", Turn: "
				<< actions::colorToName(realBoard->getPlayingColor()) << "\n\n";
		}

		auto turns = ConsoleChess::getInstance().turn();

		if (turns.second - turns.first > 8)
			turns.second = turns.first + 8;

		auto movesWidth = 21;

		std::vector<std::string> turnTable;
		turnTable.emplace_back("       Moves:");
		turnTable.emplace_back();


		for (auto& v : realBoard->getTurnInfo(turns.first, turns.second)) {
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

		static auto _formatGrave = [&](Color c, int width) {
			ProfileDeclare;

			std::vector<std::string> formatted;
			formatted.emplace_back();
			int counter = 0;
			for (auto& ps : getChessBoard()->getGraveyard(c)) {
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

		std::vector<std::string> wGraveFormatted = _formatGrave(Color::White, 14);
		std::vector<std::string> bGraveFormatted = _formatGrave(Color::Black, 14);
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

	actions::turn({ "reset" });
}

bool ConsoleChess::isActive() const
{
	return board->getWinner() != Color::None;
}
