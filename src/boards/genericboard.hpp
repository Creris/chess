#pragma once

#ifndef GENERIC_BOARD_HEADER_H_
#define GENERIC_BOARD_HEADER_H_

/*
	This file contains:
		- Definition of a generic board class GenericBoard.
*/

#include "../boardstate.hpp"

#include <memory>
#include <vector>

class PieceGeneric;

/**
	A generic board that any specific game's boards can derive from.
*/
class GenericBoard {
	void _clearThreat();
protected:
	/**
		Defines a storage information class for pointers to pieces.

		Contains information about where the piece started at as well as if
		it ever moved.
	*/
	struct PieceStorage {
		Position startingPos = { -1, -1 };
		std::shared_ptr<PieceGeneric> piecePtr = nullptr;
		bool moved = false;

		PieceStorage() {}
		PieceStorage(Position _s,
					 const std::shared_ptr<PieceGeneric>& _p) : startingPos(_s), piecePtr(_p) {}
	};

	BoardState state;
	std::vector<PieceStorage> pieces;

	/**
		White's graveyard.
	*/
	std::vector<PieceInfo> whiteGrave;

	/**
		Black's graveyard.
	*/
	std::vector<PieceInfo> blackGrave;

	PieceStorage _getPieceStorage(Position atPos) const;
	PieceStorage _getPieceStorage(Position atPos, bool isInit) const;
public:
	/**
		Construct a new instance of GenericBoard.

		The board will contain empty board state that has specified size.

		\param boardWidth Width of the board.
		\param boardHeight Height of the board.
	*/
	GenericBoard(int boardWidth, int boardHeight);

	/**
		Adds new piece to the board at specified position provided
		given info.

		The threat member of PieceInfo is ingored.

		\sa PieceInfo()
		\param position The position to add the piece on the board into.
		\param pieceInfo Piece info about the piece to put onto the board.
	*/
	void addPiece(Position position, PieceInfo pieceInfo);

	/**
		Gets the state of the board.

		\return State of the board.
	*/
	const BoardState& getState() const;

	/**
		Gets the state of the board.

		\return State of the board.
	*/
	BoardState& getState();

	/**
		Initialize the board to given state.

		The state is copied.

		\param state State to initialize this board with.
	*/
	void initialize(const BoardState& state);

	/**
		Recalculate all threats of all pieces on the board.
	*/
	void recalculateThread();

	/**
		Parse user input.

		\param userInput user input to be parsed.
		\return Whether the parser understood this command.
	*/
	virtual bool parseUserInput(const std::string& userInput) = 0;

	/**
		Get a list of pieces in given player's graveyard.

		\param forColor Color of the player to retreive the graveyard for.
		\return Vector of PieceInfo of graveyarded pieces.

		\sa Color()
		\sa PieceInfo()
	*/
	const std::vector<PieceInfo>& getGraveyard(Color forColor) const;

	/**
		Get a pointer to a piece at given position.

		\param atPos The position the piece should be standing at.
		\return Pointer to a piece standing at this position.
				If none, returns nullptr.

		\sa Position()
		\sa PieceGeneric()
	*/
	std::shared_ptr<PieceGeneric> getPiece(Position atPos) const;

	/**
		Get whether a piece at a given position moved at all.

		\param position Position to look for.
		\param isInit Whether the position is initial or current.
		
		\return Whether a piece found in that position has ever moved.
	*/
	bool didMove(Position position, bool isInit = false) const;
};

#endif // GENERIC_BOARD_HEADER_H_
