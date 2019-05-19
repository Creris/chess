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

#include "boardtype.hpp"

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

//Forward declare generic piece interface
class PieceGeneric;

/**
	Defines a storage information class for pointers to pieces.

	Contains information about where the piece started at as well as if
	it ever moved.
*/
struct PieceStorage {
	Position startingPos = { -1, -1 };
	std::shared_ptr<PieceGeneric> piecePtr = nullptr;
	std::array<std::vector<decltype(piecePtr)>, 2> threat = {};

	PieceStorage() {}
	PieceStorage(Position _s,
					const std::shared_ptr<PieceGeneric>& _p) : startingPos(_s), piecePtr(_p) {}

	~PieceStorage() = default;
	PieceStorage(const PieceStorage&) = default;
	PieceStorage(PieceStorage&&) noexcept = default;
	
	/**
		Visual studio 2019 diagnozed this function as deleted for some reason,
		even if it has = default after its declaration.
		
		This is therefore a workaround.
	*/
	PieceStorage& operator=(const PieceStorage& p) {
		startingPos = p.startingPos;
		piecePtr = p.piecePtr;
		threat = p.threat;

		return *this;
	}

	/**
		Visual studio 2019 diagnozed this function as deleted for some reason,
		even if it has = default after its declaration.

		This is therefore a workaround.
	*/
	PieceStorage& operator=(PieceStorage&& p) noexcept {
		startingPos = std::move(p.startingPos);
		piecePtr = std::move(p.piecePtr);
		threat = std::move(p.threat);

		return *this;
	}
};

/**
	Represents the state of any board that the pieces on board use for calculation
	of their moves.
*/
struct BoardState {
	int width;		/**< Width of the board. */
	int height;		/**< Height of the board. */

	BoardType type;	/**< Board type. */

	/**
		Represents the information about every square on the board.
	*/
	std::vector<std::vector<PieceStorage>> squares;
};

/**
	Get a BoardState that can be regarded as empty.

	\return New instance of a state that is regarded as empty.
	\sa isBoardStateEmpty()
*/
inline BoardState getEmptyBoardState() {
	return { 0, 0, BoardType::None, {} };
}

/**
	Test if given BoardState is empty.

	\param state A state to test for being empty.
	\return Whether given state is empty or not.
*/
inline bool isBoardStateEmpty(const BoardState& state) {
	return state.width == 0 || state.height == 0 || state.squares.size() == 0
		|| state.type == BoardType::None;
}

#endif // BOARD_STATE_HEADER_H_
