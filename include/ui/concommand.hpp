#pragma once

#ifndef CON_COMMAND_HEADER_H_
#define CON_COMMAND_HEADER_H_

#include <memory>
#include <utility>
#include <unordered_map>
#include <string>
#include <vector>

#include "conactions.hpp"

using namespace std::string_literals;

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
	{ Command::Quit, std::make_pair("quit [FILE]"s,
			"Quits the chess game. If FILE is provided, exports\n"s
			"the list of moves to that file before quiting the game."s)
	},
	{ Command::Exit, std::make_pair("exit [FILE]"s,
			"Quits the chess game. If FILE is provided, exports\n"s
			"the list of moves to that file before quiting the game."s)
	},
	{ Command::Help, std::make_pair("help\nhelp { commands }"s,
			"Prints this help message.\n"s
			"When command list is provided, displays help for\n"s
			"every command in the list.\n"s
			"Command only has to be the command without parameters."s)
	},
	{ Command::Commands, std::make_pair("commands"s,
			"Prints list of available commands with no\n"s
			"additional information."s)
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
	{ Command::Move, std::make_pair("To\nFrom To\nmove To\nmove From To"s,
			"Perform a move from position From into position To.\n"s
			"If only one coordinate is provided, attempts to move the\n"s
			"selected piece.\n"s
			"If both coordinates are provided, has same effect as\n"s
			"select From, move To.\n"s
			"If one or two coordinates are provided as input, they\n"s
			"are interpreted as move commands."s)
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

	if (map.find(input) == map.end())	return Command::Invalid;
	return map.at(input);
};

using CommandHandler = bool(*)(const std::vector<std::string_view>& args);

inline std::unordered_map<Command, CommandHandler> commandHandlers = {
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

#endif // CON_COMMAND_HEADER_H_
