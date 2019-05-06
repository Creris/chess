#pragma once

#ifndef PIECE_PAWN_HEADER_H_
#define PIECE_PAWN_HEADER_H_

/*
	This file contains:
		- Definition of a PiecePawn that derives from a generic piece.
*/

#include "generic.hpp"

class PiecePawn : public PieceGeneric {
	/*
		Get whether a position of a piece is its initial position or not.

		\return Whether the pawn is on its initial position.
	*/
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
