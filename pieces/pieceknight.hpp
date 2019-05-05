#pragma once

#ifndef PIECE_KNIGHT_HEADER_H_
#define PIECE_KNIGHT_HEADER_H_

#include "piecegeneric.hpp"
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
	virtual bool canMove(Position toPos, const BoardState& state) const override;
	virtual PieceType getType() const override;
	std::vector<Position> getAllAvailableMoves(const BoardState& state) const override;
};

#endif // PIECE_KNIGHT_HEADER_H_
