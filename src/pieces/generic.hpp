#pragma once

#ifndef GENERIC_PIECE_HEADER_H_
#define GENERIC_PIECE_HEADER_H_

/*
	This file contains:
		- Definition of PieceGeneric, which is a generic piece that all other
		  pieces should inherit from.
*/

#include "../boardstate.hpp"
#include "../piecetype.hpp"

#include <vector>
#include <algorithm>

class GenericBoard;

/**
	An interface for that all pieces should use.

	At the same time is a propagator of moves which any board can register into
	and await calls for 
*/
class PieceGeneric {
	/**
		List of listeners for events.
	*/
	std::vector<GenericBoard*> listeners;
protected:
	Position position;		/**< Position of the piece. */
	Color color;			/**< Color of the piece. */

	/**
		Forcefully performs a move of a piece.
		
		Will update the BoardState state as well as its own position.
		Will not do anything, if the new position is outside of the board.

		\param toPos Position to move into.
		\param state BoardState to perform this move over. Will be updated.
		\sa isInsideBoard()
	*/
	virtual void moveAction(Position toPos, BoardState& state);
public:
	PieceGeneric(Color c) : position{}, color(c) {}

	//Have to override these as they are essential
	/**
		Tests whether a move from current position to given position can be
		performed given a board state.

		This function is pure virtual and has to be implemented in all classes
		deriving this class.

		\sa position()
		\param toPos A position that we want to move towards.
		\param state A current state of the board that the piece should evaluate over.
		\return Whether a piece can move from its current position to given position.
	*/
	virtual bool canMove(Position toPos, const BoardState& state) const = 0;

	/** Pure virtual function that returns a type of given piece. */
	virtual PieceType getType() const = 0;


	//Does not trigger events unlike move
	/**
		Set a position of a piece to given position.
		Does not propagate events to listeners.
	*/
	void setPosition(Position newPos);

	/**
		Get position.
		\return Position of a piece.
	*/
	Position getPosition() const;

	/**
		Get a color.
		\return Color of a piece.
	*/
	Color getColor() const;

	/**
		Given a state of a board, check whether given position is inside of
		the board.

		\param pos Position to test.
		\param state A state of a baord to test the position over.
		\return True if pos is within valid position for a board with its state state.
				False otherwise.
	*/
	bool isInsideBoard(Position pos, const BoardState& state) const;

	//Can override these if the figure wants to modify this behaviour.
	/**
		Get a list of all possible upgrade options for a piece.

		By default, no piece is upgradable, but upgradable pieces can override
		this function and return a vector of available upgrades.

		\return Vector of all possible upgrades. Empty vector if none available.
	*/
	virtual std::vector<PieceType> getUpgradeOptions() const;

	/**
		Returns a list of all available moves for this piece with given board state.

		This method has to be implemented by all classes deriving from this class.
		
		\param state A board state to calculate moves over.
		\return A vector of all possible moves for this piece on a board represented by this state.
	*/
	virtual std::vector<Position> getAllAvailableMoves(const BoardState& state) const = 0;

	/**
		Attempt to perform a move of this piece to new position on a board
		represented by a given state.

		If the position is not valid(Either outside of the board, or the piece
		cannot reach it), will not modify the board state.

		Will trigger all preMove events on all listeners before performing the move,
		and will trigger all postMove events on all listeners after doing it.

		\sa canMove()
		\sa isInsideBounds()
		\sa moveAction()
		\return Whether the move was sucessful or not.
	*/
	bool move(Position toPos, BoardState& state);
};

#endif // GENERIC_PIECE_HEADER_H_
