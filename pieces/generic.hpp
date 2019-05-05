#pragma once

#ifndef GENERIC_PIECE_HEADER_H_
#define GENERIC_PIECE_HEADER_H_

#include "../boardstate.hpp"
#include "../piecetype.hpp"
#include "../movepropagator.hpp"
#include "../movelistener.hpp"

#include <vector>
#include <algorithm>

class PieceGeneric : public MovePropagator {
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
	Position position;
	Color color;
	int _move_sentry = 0;
public:
	PieceGeneric(Color c) : position{}, color(c) {}
	~PieceGeneric() = default;
	PieceGeneric(const PieceGeneric&) = default;
	PieceGeneric(PieceGeneric&&) noexcept = default;
	PieceGeneric& operator=(const PieceGeneric&) = default;
	PieceGeneric& operator=(PieceGeneric&&) noexcept = default;

	//Have to override these as they are essential
	virtual bool canMove(Position toPos, const BoardState& state) const = 0;
	virtual PieceType getType() const = 0;

	//Does not trigger events unlike move
	void setPosition(Position newPos) {
		position = newPos;
	}

	Position getPosition() const {
		return position;
	}

	Color getColor() const {
		return color;
	}

	bool isInsideBoard(Position pos, const BoardState& state) const {
		return pos.first >= 0 && pos.second >= 0 &&
			pos.first < state.width && pos.second < state.height;
	}
	

	//Can override these if the figure wants to modify this behaviour.
	
	/*
		Calling from outside of the class will not do anyhting.
		Has to be called from within the class, such as via move().
	*/
	virtual BoardState moveAction(Position toPos, const BoardState& state) {
		//TODO: Actually implement the move so that the figure updates itself
		//		as well as the map

		//Disallow call to moveAction outside of the class
		if (!_move_sentry)
			return state;
		return state;
	}
	
	virtual std::vector<PieceType> getUpgradeOptions() const {
		return {};
	}

	virtual std::vector<Position> getAllAvailableMoves(const BoardState& state) const = 0;


	/*
		Calls all preMoves registered, executes moveAction and calls postMoves.
		If the move is not valid move, returns empty state instead.
	*/
	BoardState move(Position toPos, const BoardState& state) {
		BoardState s = getEmptyBoardState();
		//If the move is not valid, ignore it and return empty state
		//to make sure the caller knows this was not successful
		if (this->canMove(toPos, state)) {
			//Speical RAII wrapper for setting _move_sentry back to 0
			//no matter how we exit this if block.
			class _R { int& v; public:
				_R(int& _) : v(_) {}
				~_R() { v = 0; }
			};
			
			//Construct it and update _move_sentry
			_R raii{ _move_sentry };
			_move_sentry = 1;

			//Execute
			_execPre();
			s = this->moveAction(toPos, state);
			_execPost();
		}
		return s;
	}

	// Inherited via MovePropagator
	void addListener(const std::shared_ptr<MoveListener>& listener) override {
		listeners.push_back(listener);
	}

	void removeListener(const std::shared_ptr<MoveListener>& listener) override {
		listeners.erase(std::remove(listeners.begin(), listeners.end(), listener),
						listeners.end());
	}
	void clearListeners() override {
		listeners = {};
	}
};

#endif // GENERIC_PIECE_HEADER_H_
