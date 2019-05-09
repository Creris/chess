#include "genericboard.hpp"

#include "../pieces/piecebuilder.hpp"

void GenericBoard::_clearThreat()
{
	for (auto& row : state.squares)
		for (auto& cell : row) {
			cell.threat.fill(0);
		}
}

GenericBoard::GenericBoard(int boardWidth, int boardHeight) : state{ boardWidth, boardHeight, {} }
{
}

inline void GenericBoard::addPiece(Position position, PieceInfo pieceInfo) {
	auto piece = newPieceByType(pieceInfo, position);
	if (!piece)	return;
	piece->addListener(this);
	pieces.push_back(std::move(piece));
}

inline const BoardState& GenericBoard::getState() const {
	return state;
}

void GenericBoard::initialize(const BoardState& state)
{
	if (state.width == this->state.width && state.height == this->state.height)
		this->state = state;
}

void GenericBoard::recalculateThread()
{
	_clearThreat();
	auto cToIdx = [](Color c) {
		return c == Color::White ? 0 : 1;
	};
	for (auto& piece : pieces) {
		for (auto& p : piece->getAllAvailableMoves(state)) {
			auto& square = state.squares[p.first][p.second];
			square.threat[cToIdx(piece->getColor())]++;
		}
	}
}
