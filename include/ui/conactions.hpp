#pragma once

#ifndef CONSOLE_CHESS_ACTIONS_HEADER_H_
#define CONSOLE_CHESS_ACTIONS_HEADER_H_

#include "../piecetype.hpp"
#include "../boardstate.hpp"
#include "../boards/genericboard.hpp"

#include <vector>
#include <string_view>
#include <string>

inline auto typeToName = [](PieceType type) {
	using namespace std::string_literals;

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

inline auto colorToName = [](Color c) {
	using namespace std::string_literals;

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

inline auto pieceToName = [](PieceType type, Color c) {
	auto typeName = typeToName(type);
	auto colorName = colorToName(c);

	if (c == Color::Black && typeName.size() && typeName.back() == ']')
		typeName[typeName.size() - 2] = std::tolower(typeName[typeName.size() - 2]);

	return typeName;
};

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

inline auto typeToCharRaw = [](PieceType type) {
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

namespace actions {
	using namespace std::string_literals;

	bool help(			GenericBoard& board, const std::vector<std::string_view>& args);
	bool _internalHelp(	GenericBoard& board, const std::vector<std::string_view>& args);
	bool export_moves(	GenericBoard& board, const std::vector<std::string_view>& args);
	bool commands(		GenericBoard& board, const std::vector<std::string_view>& args);
	bool quit(			GenericBoard& board, const std::vector<std::string_view>& args);
	bool grid(			GenericBoard& board, const std::vector<std::string_view>& args);
	bool render(		GenericBoard& board, const std::vector<std::string_view>& args);
	bool turn(			GenericBoard& board, const std::vector<std::string_view>& args);
	bool restart(		GenericBoard& board, const std::vector<std::string_view>& args);
	bool select(		GenericBoard& board, const std::vector<std::string_view>& args);
	bool unselect(		GenericBoard& board, const std::vector<std::string_view>& args);
	bool info(			GenericBoard& board, const std::vector<std::string_view>& args);
	bool forfeit(		GenericBoard& board, const std::vector<std::string_view>& args);
	bool move(			GenericBoard& board, const std::vector<std::string_view>& args);
	bool profile(		GenericBoard& board, const std::vector<std::string_view>& args);
}


#endif // CONSOLE_CHESS_ACTIONS_HEADER_H_
