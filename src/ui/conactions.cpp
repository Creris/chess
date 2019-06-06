#include "../../include/ui/conactions.hpp"
#include "../../include/ui/conchess.hpp"
#include "../../include/profiler.hpp"

#include <vector>
#include <fstream>
#include <string>
#include <string_view>
#include <iostream>
#include <iomanip>

namespace actions {
	bool help(GenericBoard& board, const std::vector<std::string_view>& args) {
		ProfileDeclare;
		ConsoleChess::getInstance().help(join(args, " "));
		return false;
	}

	bool _internalHelp(GenericBoard& board, const std::vector<std::string_view>& args) {
		ProfileDeclare;
		std::cout << "Invalid arguments for command " << args[0] << ".\n";
		ConsoleChess::getInstance().help(join(args, ""));
		return true;
	}

	bool export_moves(GenericBoard& board, const std::vector<std::string_view>& args) {
		ProfileDeclare;
		if (args.size() != 1)	return _internalHelp(board, { "export" });
		std::ofstream moves{ std::string{ args[0] }, std::ios_base::binary };
		auto allTurns = board.getTurnInfo(1, board.getTurn());
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

	bool commands(GenericBoard& board, const std::vector<std::string_view>& args) {
		ProfileDeclare;
		if (args.size())	return _internalHelp(board, { "commands" });
		listCommands();
		return false;
	}

	bool quit(GenericBoard& board, const std::vector<std::string_view>& args) {
		ProfileDeclare;
		if (args.size() > 2)	return _internalHelp(board, { "quit" });
		else if (args.size() == 1) {
			export_moves(board, args);
		}
		std::cout << "Quiting chess.\n";
		exit(0);
	}

	bool grid(GenericBoard& board, const std::vector<std::string_view>& args) {
		ProfileDeclare;
		if (args.size() != 1)	return _internalHelp(board, { "grid" });
		int size = -1;
		try {
			size = std::stoi(std::string{ args[0] });
		} catch (std::invalid_argument&) {
			return _internalHelp(board, { "grid" });
		} catch (std::out_of_range&) {
			return _internalHelp(board, { "grid" });
		} catch (...) {
			std::cout << "Unexpected exception while handling grid rescaling.\n\n";
			exit(0);
		}
		if (!ConsoleChess::getInstance().grid(size))
			return _internalHelp(board, { "grid" });

		return true;
	}

	bool render(GenericBoard& board, const std::vector<std::string_view>& args) {
		ProfileDeclare;
		if (args.size())	return _internalHelp(board, { "render" });
		ConsoleChess::getInstance().render();
		std::cout << "\n";
		return false;
	}

	bool turn(GenericBoard& board, const std::vector<std::string_view>& args) {
		ProfileDeclare;
		if (args.size() > 2 || !args.size())	return _internalHelp(board, { "turn" });
		if (args[0] == "reset") {
			auto current = board.getTurn();
			ConsoleChess::getInstance().turn(std::max(current - 8, 1), current);
			return true;
		}

		int turnU = 0;
		int turnV = -1;
		bool turnVIn = false;
		int maxTurn = board.getTurn();

		try {
			turnU = std::stoi(std::string{ args[0] });
			if (args.size() == 2) {
				turnV = std::stoi(std::string{ args[1] });
				turnVIn = true;
			}
		} catch (...) {
			return _internalHelp(board, { "turn" });
		}

		if (turnU > maxTurn || turnU < 1)
			return _internalHelp(board, { "turn" });

		if (!turnVIn) {

			auto info = board.getTurnInfo(turnU);
			std::cout << "  " << turnU << ". " << info.first << "  " << info.second << "\n\n";
			return false;
		}

		if (turnV > maxTurn || turnV < turnU)
			return _internalHelp(board, { "turn" });

		ConsoleChess::getInstance().turn(turnU, turnV);

		return true;
	}

	bool restart(GenericBoard& board, const std::vector<std::string_view>& args) {
		ProfileDeclare;
		if (args.size())	return _internalHelp(board, { "restart" });
		board.initialize();
		return true;
	}

	bool select(GenericBoard& board, const std::vector<std::string_view>& args) {
		ProfileDeclare;
		if (args.size() != 1) {
			return _internalHelp(board, { "select" });
		}
		Position pos = stringToPosition(args[0]);
		if (pos.first == -1 || pos.second == -1) {
			return _internalHelp(board, { "select" });
		}

		if (!board.select(pos)) {
			std::cout << "Can't select piece at " << args[0] << ".";
			auto piece = board.getPiece(pos);
			if (!piece) {
				std::cout << " No piece found at this position.\n";
			}
			else if (piece->getType() == PieceType::ShadowPawn ||
					 piece->getType() == PieceType::None) {
				std::cout << " No piece found at this position.\n";
			}
			else if (piece->getColor() != board.getPlayingColor()) {
				std::cout << " Piece is not owned by the playing player.\n";
			}
			else {
				std::cout << " Unknown reason.\n";
			}
			std::cout << "\n";
		}

		return true;
	}

	bool unselect(GenericBoard& board, const std::vector<std::string_view>& args) {
		ProfileDeclare;
		if (args.size())	return _internalHelp(board, { "unselect" });
		board.unselect();
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

	std::vector<Position> _getSelfCheckPositions(GenericBoard& board, Position atPosition,
												 const std::vector<Position>& valid) {
		ProfileDeclare;
		auto& storage = board.getPieceStorage(atPosition);
		if (!storage.piecePtr)	return {};
		if (storage.piecePtr->getType() == PieceType::ShadowPawn ||
			storage.piecePtr->getType() == PieceType::None)
			return {};

		auto allMoves = storage.piecePtr->getAllAvailableMoves(atPosition, board.getState());
		allMoves.erase(std::remove_if(allMoves.begin(), allMoves.end(), [&](const Position& p) {
			return std::find(valid.begin(), valid.end(), p) != valid.end();
									  }), allMoves.end());

		return allMoves;
	}

	void _printPossibleMoves(GenericBoard& board, Position pos) {
		ProfileDeclare;
		if (pos.first == -1 || pos.second == -1)	return;
		auto& piece = board.getPieceStorage(pos);

		auto valid = board.getPossibleMoves(pos);
		auto invalid = _getSelfCheckPositions(board, pos, valid);

		static auto _cleanBack = [](std::string& s) {
			if (s.size() >= 2 && s[s.size() - 1] == ' ' && s[s.size() - 2] == ',') {
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
				auto target = board.getPiece(move);
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
				auto target = board.getPiece(move);
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

	bool info(GenericBoard& board, const std::vector<std::string_view>& args) {
		ProfileDeclare;
		if (args.size() != 1)	return _internalHelp(board, { "info" });
		Position pos = stringToPosition(args[0]);
		if (pos.first == -1 || pos.second == -1)	return _internalHelp(board, { "info" });
		auto& storage = board.getPieceStorage(pos);
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
		int whiteAttacking = static_cast<int>(storage.threat[0].size());
		int blackAttacking = static_cast<int>(storage.threat[1].size());

		std::cout << "  Piece name: " << typeName;
		if (type != PieceType::None && type != PieceType::ShadowPawn)
			std::cout << ", Piece color: " << colorName;
		std::cout << "\n";

		if (type != PieceType::ShadowPawn && type != PieceType::None) {
			std::cout << "  Piece at this position has following moves available:\n";
			_printPossibleMoves(board, pos);
		}

		std::cout << "\n  Threats:    " << whiteAttacking << " White "
			<< words[whiteAttacking != 1] << " attacking this square";


		std::string attMsg;
		for (auto& attacking : storage.threat[0]) {
			attMsg += typeToChar(attacking.second) + " from "s + positionToString(attacking.first) + ",\n";
		}

		if (attMsg != "") {
			std::cout << ":\n                 ";
			std::cout << join(split(strip(attMsg, ",\n"), "\n"), "\n                 ") << "\n";
		}
		else {
			std::cout << ".\n";
		}

		std::cout << "\n              " << blackAttacking << " Black "
			<< words[blackAttacking != 1] << " attacking this square";

		attMsg = "";
		for (auto& attacking : storage.threat[1]) {
			attMsg += typeToChar(attacking.second) + " from "s + positionToString(attacking.first) + ",\n";
		}

		if (attMsg != "") {
			std::cout << ":\n                 ";
			std::cout << join(split(strip(attMsg, ",\n"), "\n"), "\n                 ") << "\n";
		}
		else {
			std::cout << ".\n";
		}
		std::cout << "\n";

		return true;
	}

	bool forfeit(GenericBoard& board, const std::vector<std::string_view>& args) {
		ProfileDeclare;
		if (args.size())	return _internalHelp(board, { "forfeit" });
		std::cout << "Player " << colorToName(board.getPlayingColor()) << " forfeits!\n";
		board.forfeit();
		render(board, {});

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
					return restart(board, {});
				case 'q':
					return quit(board, {});
				case 'e':
				{
					auto split_ = split(std::string_view{ input });
					split_.erase(split_.begin());
					export_moves(board, split_);
				}
			}
		} while (true);
		return true;
	}

	bool _moveSelected(GenericBoard& board, std::string_view to) {
		ProfileDeclare;
		auto selected = board.getSelectedPosition();
		if (selected.first == -1 || selected.second == -1)
			return _internalHelp(board, { "move" });

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

		if (!board.tryMove(pos, promotion)) {
			std::cout << "This move cannot be performed. Try again.\n\n";
			return false;
		}
		unselect(board, {});
		turn(board, { "reset" });
		return true;
	}

	bool _moveSelect(GenericBoard& board, std::string_view from, std::string_view to) {
		ProfileDeclare;
		Position fromPos = stringToPosition(from);
		if (fromPos.first == -1 || fromPos.second == -1) {
			std::cout << "Invalid starting position " << from << "\n\n";
			return false;
		}
		board.select(fromPos);
		bool did = _moveSelected(board, to);
		unselect(board, {});
		return did;
	}

	bool move(GenericBoard& board, const std::vector<std::string_view>& args) {
		ProfileDeclare;
		if (args.size() == 1)		return _moveSelected(board, args[0]);
		else if (args.size() == 2)	return _moveSelect(board, args[0], args[1]);
		else						return _internalHelp(board, { "move" });
	}

	bool profile(GenericBoard& board, const std::vector<std::string_view>& args) {
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
				return _internalHelp(board, { "profile" });
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
				else return _internalHelp(board, { "profile" });
			}

			Profiler::target(std::string{ rest[0] });
			return true;
		}

		return _internalHelp(board, { "profile" });
	}
}