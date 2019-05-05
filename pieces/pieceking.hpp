#pragma once

#ifndef PIECE_KING_HEADER_H_
#define PIECE_KING_HEADER_H_

#include "piecegeneric.hpp"

class PieceKing : public PieceGeneric {
public:
	PieceKing(Color c) : PieceGeneric(c) {}
	~PieceKing() = default;
	PieceKing(const PieceKing&) = default;
	PieceKing(PieceKing&&) noexcept = default;
	PieceKing& operator=(const PieceKing&) = default;
	PieceKing& operator=(PieceKing&&) noexcept = default;

	// Inherited via PieceGeneric
	virtual bool canMove(Position toPos, const BoardState& state) const override;
	virtual PieceType getType() const override;

	std::vector<Position> getAllAvailableMoves(const BoardState& state) const override;
};

#endif // PIECE_KING_HEADER_H_
