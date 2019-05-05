#pragma once

#ifndef PIECE_QUEEN_HEADER_H_
#define PIECE_QUEEN_HEADER_H_

#include "piecegeneric.hpp"

class PieceQueen : public PieceGeneric {
public:
	PieceQueen(Color c) : PieceGeneric(c) {}
	~PieceQueen() = default;
	PieceQueen(const PieceQueen&) = default;
	PieceQueen(PieceQueen&&) noexcept = default;
	PieceQueen& operator=(const PieceQueen&) = default;
	PieceQueen& operator=(PieceQueen&&) noexcept = default;

	// Inherited via PieceGeneric
	virtual bool canMove(Position toPos, const BoardState& state) const override;
	virtual PieceType getType() const override;
	virtual std::vector<Position> getAllAvailableMoves(const BoardState& state) const override;
};

#endif // PIECE_QUEEN_HEADER_H_
