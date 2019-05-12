#pragma once

#ifndef BOARD_STATE_HEADER_H_
#define BOARD_STATE_HEADER_H_

/*
	This header file contains:
		- Definition for a Position type, used by all boards.
		- Definition of a color used by all boards.
		- Definition of a BoardState, a class that represents a state
		  of a board.

	For more information refer to the comments above specific declarations.
*/

#include <vector>
#include <cstdint>
#include "piecetype.hpp"
#include <stdexcept>
#include <utility>
#include <array>

/**
	Represents a position on a board.
	Used throughout the program as a type for a position on a board.
	
	Since this is typedefed to std::pair, the elements are `first` and `second`.

	Member `first` represents a 'rank', a row, on a board.
	Member `second` represents a 'file', a column on a board.
*/
using Position = std::pair<int8_t, int8_t>;

/**
	Represents a color for a player and pieces.
	Every piece has exactly one color.

	None color should only be used on squares on a board that contain on pieces.
*/
enum class Color {
	Black,
	White,
	None,
};

/**
	Information about a square on the board that is used by BoardState.

	Contains a type of a piece and its color.
	
	Note that even if type == PieceType::None, the color might not necessarily
	be equal to None.
*/
struct PieceInfo {
	PieceType type;
	Color color;
};

/**
	Contains information about each square on a board.

	Includes piece information and threat on each piece by both player colors.
*/
struct SquareInfo {
	/**
		Information about the piece at this square.
	*/
	PieceInfo piece;

	/**
		How many pieces are attacking this square.

		[0] = white attackers.
		[1] = black attackers.
	*/
	std::array<int8_t, 2> threat;
};

/**
	Represents the state of any board that the pieces on board use for calculation
	of their moves.
*/
struct BoardState {
	int width;		/** Width of the board. */
	int height;		/** Height of the board. */

	/**
		Represents the information about every square on the board.
	*/
	std::vector<std::vector<SquareInfo>> squares;
};

/**
	Get a BoardState that can be regarded as empty.

	\return New instance of a state that is regarded as empty.
	\sa isBoardStateEmpty()
*/
inline BoardState getEmptyBoardState() {
	return { 0, 0, {} };
}

/**
	Test if given BoardState is empty.

	\param state A state to test for being empty.
	\return Whether given state is empty or not.
*/
inline bool isBoardStateEmpty(const BoardState& state) {
	return state.width == 0 || state.height == 0 || state.squares.size() == 0;
}

#endif // BOARD_STATE_HEADER_H_
