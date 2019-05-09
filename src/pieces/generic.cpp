#include "generic.hpp"
#include "../boards/genericboard.hpp"

void PieceGeneric::_execPre(const Position& target) {
	for (auto& ptr : listeners)
		ptr->preMove(this, target);
}

void PieceGeneric::_execPost(const Position& origin) {
	for (auto& ptr : listeners)
		ptr->postMove(this, origin);
}

void PieceGeneric::moveAction(Position toPos, BoardState& state) {
	if (!isInsideBoard(toPos, state))
		return;

	state.squares[toPos.first][toPos.second] = { getType(), getColor() };
	state.squares[position.first][position.second] = { PieceType::None, Color::None };
	position = toPos;
}

void PieceGeneric::setPosition(Position newPos) {
	position = newPos;
}

Position PieceGeneric::getPosition() const {
	return position;
}

Color PieceGeneric::getColor() const {
	return color;
}

bool PieceGeneric::isInsideBoard(Position pos, const BoardState& state) const {
	return pos.first >= 0 && pos.second >= 0 &&
		pos.first < state.width && pos.second < state.height;
}

std::vector<PieceType> PieceGeneric::getUpgradeOptions() const {
	return {};
}

bool PieceGeneric::move(Position toPos, BoardState& state) {
	//If the move is not valid, ignore it and return empty state
	//to make sure the caller knows this was not successful
	if (this->canMove(toPos, state)) {
		_execPre(toPos);
		Position oldPos = position;
		this->moveAction(toPos, state);
		_execPost(position);
		return true;
	}
	return false;
}


void PieceGeneric::addListener(GenericBoard* listener) {
	listeners.push_back(listener);
}

void PieceGeneric::removeListener(GenericBoard* listener) {
	listeners.erase(std::remove(listeners.begin(), listeners.end(), listener),
		listeners.end());
}

void PieceGeneric::clearListeners() {
	listeners = {};
}
