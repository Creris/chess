#pragma once

#ifndef PIECE_BISHOP_HEADER_H_
#define PIECE_BISHOP_HEADER_H_

#include "generic.hpp"

class PieceBishop : public PieceGeneric {
public:
	PieceBishop(Color c) : PieceGeneric(c) {}
	~PieceBishop() = default;
	PieceBishop(const PieceBishop&) = default;
	PieceBishop(PieceBishop&&) noexcept = default;
	PieceBishop& operator=(const PieceBishop&) = default;
	PieceBishop& operator=(PieceBishop&&) noexcept = default;

	// Inherited via PieceGeneric
	virtual bool canMove(Position toPos, const BoardState& state) const override;
	virtual PieceType getType() const override;
	virtual std::vector<Position> getAllAvailableMoves(const BoardState& state) const override;
};

#endif // PIECE_BISHOP_HEADER_H_
