#pragma once

#ifndef PIECE_PAWN_HEADER_H_
#define PIECE_PAWN_HEADER_H_

#include "piecegeneric.hpp"

class PiecePawn : public PieceGeneric {
	bool isInitialPosition() const {
		return (color == Color::Black && position.first == 6 ||
			color == Color::White && position.first == 1);
	};
public:
	PiecePawn(Color c) : PieceGeneric(c) {}
	~PiecePawn() = default;
	PiecePawn(const PiecePawn&) = default;
	PiecePawn(PiecePawn&&) noexcept = default;
	PiecePawn& operator=(const PiecePawn&) = default;
	PiecePawn& operator=(PiecePawn&&) noexcept = default;

	// Inherited via PieceGeneric
	virtual bool canMove(Position toPos, const BoardState& state) const override;
	virtual PieceType getType() const override;
	virtual std::vector<Position> getAllAvailableMoves(const BoardState& state) const override;
};

#endif // PIECE_PAWN_HEADER_H_
