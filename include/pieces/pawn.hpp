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
	bool isInitialPosition(Position fromPos) const {
		return (color == Color::Black && fromPos.first == 6 ||
			color == Color::White && fromPos.first == 1);
	};

	void moveAction(Position fromPos, Position toPos, BoardState& state) const override;
public:
	PiecePawn(Color c) : PieceGeneric(c) {}
	~PiecePawn() = default;
	PiecePawn(const PiecePawn&) = default;
	PiecePawn(PiecePawn&&) noexcept = default;
	PiecePawn& operator=(const PiecePawn&) = default;
	PiecePawn& operator=(PiecePawn&&) noexcept = default;

	// Inherited via PieceGeneric
	bool canMove(Position fromPos, Position toPos, const BoardState& state) const override;
	PieceType getType() const override;
	
	std::vector<Position> getAllAvailableMoves(Position fromPos, const BoardState& state) const override;
	std::vector<Position> getAllThreateningMoves(Position fromPos, const BoardState& state) const override;

	std::vector<PieceType> getUpgradeOptions() const override;
	
	std::pair<bool, PieceStorage> move(Position fromPos, Position toPos, BoardState& state) const override;
};

#endif // PIECE_PAWN_HEADER_H_
