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
#include "../movepropagator.hpp"
#include "../movelistener.hpp"

#include <vector>
#include <algorithm>

/**
	An interface for that all pieces should use.

	Is a child of MovePropagator and implements the interface.
	This means that this class, and all children of this class will be
	MovePropagators and therefore will be propagating events via Observer pattern
	to all registered MoveListeners.

	\sa MovePorpagator()
	\sa MoveListener()
*/
class PieceGeneric : public MovePropagator {
	/**
		List of listeners for events.
	*/
	std::vector<std::shared_ptr<MoveListener>> listeners;

	void _execPre() {
		for (auto& ptr : listeners)
			ptr->preMove(this);
	}

	void _execPost() {
		for (auto& ptr : listeners)
			ptr->postMove(this);
	}
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
	virtual void moveAction(Position toPos, BoardState& state) {
		if (!isInsideBoard(toPos, state))
			return;

		state.squares[toPos.first][toPos.second] = { getType(), getColor() };
		state.squares[position.first][position.second] = { PieceType::None, Color::None };
		position = toPos;
	}
public:
	PieceGeneric(Color c) : position{}, color(c) {}
	~PieceGeneric() = default;
	PieceGeneric(const PieceGeneric&) = default;
	PieceGeneric(PieceGeneric&&) noexcept = default;
	PieceGeneric& operator=(const PieceGeneric&) = default;
	PieceGeneric& operator=(PieceGeneric&&) noexcept = default;

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
	void setPosition(Position newPos) {
		position = newPos;
	}

	/**
		Get position.
		\return Position of a piece.
	*/
	Position getPosition() const {
		return position;
	}

	/**
		Get a color.
		\return Color of a piece.
	*/
	Color getColor() const {
		return color;
	}

	/**
		Given a state of a board, check whether given position is inside of
		the board.

		\param pos Position to test.
		\param state A state of a baord to test the position over.
		\return True if pos is within valid position for a board with its state state.
				False otherwise.
	*/
	bool isInsideBoard(Position pos, const BoardState& state) const {
		return pos.first >= 0 && pos.second >= 0 &&
			pos.first < state.width && pos.second < state.height;
	}
	

	//Can override these if the figure wants to modify this behaviour.
	/**
		Get a list of all possible upgrade options for a piece.

		By default, no piece is upgradable, but upgradable pieces can override
		this function and return a vector of available upgrades.

		\return Vector of all possible upgrades. Empty vector if none available.
	*/
	virtual std::vector<PieceType> getUpgradeOptions() const {
		return {};
	}

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
	bool move(Position toPos, BoardState& state) {
		//If the move is not valid, ignore it and return empty state
		//to make sure the caller knows this was not successful
		if (this->canMove(toPos, state)) {
			_execPre();
			this->moveAction(toPos, state);
			_execPost();
			return true;
		}
		return false;
	}

	// Inherited via MovePropagator
	void addListener(const std::shared_ptr<MoveListener>& listener) override {
		listeners.push_back(listener);
	}

	/**
		Remove event listener from list of listeners.
		Removes all instances of this listener from the list, does nothing if none
		are found.

		\param listener Event listener to remove from the list of listeners.
	*/
	void removeListener(const std::shared_ptr<MoveListener>& listener) override {
		listeners.erase(std::remove(listeners.begin(), listeners.end(), listener),
						listeners.end());
	}

	/** Remove all listeners. */
	void clearListeners() override {
		listeners = {};
	}
};

#endif // GENERIC_PIECE_HEADER_H_
