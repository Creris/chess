#pragma once

#ifndef PIECE_ROOK_HEADER_H_
#define PIECE_ROOK_HEADER_H_

#include "generic.hpp"

class PieceRook : public PieceGeneric {
public:
	PieceRook(Color c) : PieceGeneric(c) {}
	~PieceRook() = default;
	PieceRook(const PieceRook&) = default;
	PieceRook(PieceRook&&) noexcept = default;
	PieceRook& operator=(const PieceRook&) = default;
	PieceRook& operator=(PieceRook&&) noexcept = default;

	// Inherited via PieceGeneric
	virtual bool canMove(Position toPos, const BoardState& state) const override;
	virtual PieceType getType() const override;
	virtual std::vector<Position> getAllAvailableMoves(const BoardState& state) const override;
};

#endif // PIECE_ROOK_HEADER_H_
