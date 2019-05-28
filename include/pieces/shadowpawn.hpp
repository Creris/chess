#pragma once

#ifndef SHADOW_PAWN_HEADER_H_
#define SHADOW_PAWN_HEADER_H_

#include "generic.hpp"

class PieceShadowPawn : public PieceGeneric {
public:
	PieceShadowPawn(Color c) : PieceGeneric(c) {}
	PieceType getType() const override {
		return PieceType::ShadowPawn;
	}
};

#endif // SHADOW_PAWN_HEADER_H_
