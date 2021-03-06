#pragma once

#ifndef PIECE_KNIGHT_HEADER_H_
#define PIECE_KNIGHT_HEADER_H_

/**
	This file contains:
		- Definition of a Knight piece derived from a generic piece.
*/

#include "generic.hpp"
#include <vector>

class PieceKnight : public PieceGeneric {
	/*
		Returns all possible moves a knight can make with relative offset disregarding
		bounds or collision.
	*/
	const std::vector<Position>& _allPossibleMoves() const;
public:
	PieceKnight(Color c) : PieceGeneric(c) {}
	~PieceKnight() = default;
	PieceKnight(const PieceKnight&) = default;
	PieceKnight(PieceKnight&&) noexcept = default;
	PieceKnight& operator=(const PieceKnight&) = default;
	PieceKnight& operator=(PieceKnight&&) noexcept = default;

	// Inherited via PieceGeneric
	bool canMove(Position fromPos, Position toPos, const BoardState& state) const override;
	PieceType getType() const override;
	std::vector<Position> getAllAvailableMoves(Position fromPos, const BoardState& state) const override;
};

#endif // PIECE_KNIGHT_HEADER_H_
