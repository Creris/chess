#pragma once

#ifndef PIECE_ROOK_HEADER_H_
#define PIECE_ROOK_HEADER_H_

/*
	This file contains:
		- Definition of PieceRook that derives from a generic piece.
*/

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
	bool canMove(Position fromPos, Position toPos, const BoardState& state) const override;
	PieceType getType() const override;
	std::vector<Position> getAllAvailableMoves(Position fromPos, const BoardState& state) const override;
};

#endif // PIECE_ROOK_HEADER_H_
