#include "generic.hpp"
#include "../boards/genericboard.hpp"

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
		this->moveAction(toPos, state);
		return true;
	}
	return false;
}