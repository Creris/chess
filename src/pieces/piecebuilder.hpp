#pragma once

#ifndef PIECE_BUILDER_HEADER_H_
#define PIECE_BUILDER_HEADER_H_

#include "../piecetype.hpp"
#include "generic.hpp"

#include <memory>

std::shared_ptr<PieceGeneric> newPieceByType(PieceType type, Color c = Color::None);

#endif // PIECE_BUILDER_HEADER_H_
