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
protected:
	Color color;			/**< Color of the piece. */

	/**
		Forcefully performs a move of a piece.
		
		Will update the BoardState state as well as its own position.
		Will not do anything, if the new position is outside of the board.

		\param fromPos Position to move from.
		\param toPos Position to move into.
		\param state BoardState to perform this move over. Will be updated.
		\sa isInsideBoard()
	*/
	virtual void moveAction(Position fromPos, Position toPos, BoardState& state) const;
public:
	PieceGeneric(Color c) : color(c) {}
	virtual ~PieceGeneric() = default;

	//Have to override these as they are essential
	/**
		Tests whether a move from current position to given position can be
		performed given a board state.

		This function is pure virtual and has to be implemented in all classes
		deriving this class.

		\param fromPos A position that we want to move from.
		\param toPos A position that we want to move towards.
		\param state A current state of the board that the piece should evaluate over.
		\return Whether a piece can move from its current position to given position.

		\sa Position()
	*/
	virtual bool canMove(Position fromPos, Position toPos, const BoardState& state) const;

	/** Pure virtual function that returns a type of given piece. */
	virtual PieceType getType() const;

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
	static bool isInsideBoard(Position pos, const BoardState& state);

	
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

		Ignores moves that would lead to checkmate.
		
		\param fromPos Position to get all available moves from.
		\param state A board state to calculate moves over.
		\return A vector of all possible moves for this piece on a board represented by this state.
	*/
	virtual std::vector<Position> getAllAvailableMoves(Position fromPos, 
													   const BoardState& state) const;

	virtual std::vector<Position> getAllThreateningMoves(Position fromPos,
														 const BoardState& state) const;

	/**
		Attempt to perform a move of this piece to new position on a board
		represented by a given state.

		If the position is not valid(Either outside of the board, or the piece
		cannot reach it), will not modify the board state.

		\param fromPos Position to move from.
		\param toPos Position to move towards.
		\param state The board state representing the board.
		\return Whether the move was sucessful or not.

		\sa canMove()
		\sa isInsideBounds()
		\sa moveAction()
	*/
	virtual std::pair<bool, PieceStorage> move(Position fromPos,
											   Position toPos, BoardState& state) const;
};

#endif // GENERIC_PIECE_HEADER_H_
