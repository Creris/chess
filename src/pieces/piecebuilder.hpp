#pragma once

#ifndef PIECE_BUILDER_HEADER_H_
#define PIECE_BUILDER_HEADER_H_

#include "../piecetype.hpp"
#include "generic.hpp"

#include <memory>

std::shared_ptr<PieceGeneric> newPieceByType(PieceInfo fromInfo, Position position = { 0, 0 });
std::shared_ptr<PieceGeneric> newPieceByType(PieceType type, Position position = { 0, 0 },
												Color c = Color::None);

#endif // PIECE_BUILDER_HEADER_H_
