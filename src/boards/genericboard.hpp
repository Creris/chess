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
	BoardState state;
	std::vector<std::shared_ptr<PieceGeneric>> pieces;

	/**
		White's graveyard.
	*/
	std::vector<PieceInfo> whiteGrave;

	/**
		Black's graveyard.
	*/
	std::vector<PieceInfo> blackGrave;
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
		Executed right before a move happens.

		\param origin The origin of this call.
		\param targetPos The position towards which the MovePropagator is moving.
	*/
	virtual void preMove(PieceGeneric* const origin, Position targetPos) = 0;

	/**
		Executed right after a move happens.

		\param origin The origin of this call.
		\param originPos The position from which the MovePropagator moved.
	*/
	virtual void postMove(PieceGeneric* const origin, Position originPos) = 0;
};

#endif // GENERIC_BOARD_HEADER_H_
