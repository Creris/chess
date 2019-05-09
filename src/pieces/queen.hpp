#pragma once

#ifndef PIECE_QUEEN_HEADER_H_
#define PIECE_QUEEN_HEADER_H_

/*
	This file contains:
		- Definition of PieceQueen which derives from a generic piece.
*/

#include "generic.hpp"

class PieceQueen : public PieceGeneric {
public:
	PieceQueen(Color c) : PieceGeneric(c) {}
	~PieceQueen() = default;
	PieceQueen(const PieceQueen&) = default;
	PieceQueen(PieceQueen&&) noexcept = default;
	PieceQueen& operator=(const PieceQueen&) = default;
	PieceQueen& operator=(PieceQueen&&) noexcept = default;

	// Inherited via PieceGeneric
	bool canMove(Position toPos, const BoardState& state) const override;
	PieceType getType() const override;
	std::vector<Position> getAllAvailableMoves(const BoardState& state) const override;
};

#endif // PIECE_QUEEN_HEADER_H_
