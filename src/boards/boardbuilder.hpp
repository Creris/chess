#pragma once

#ifndef BOARD_BUILDER_HEADER_H_
#define BOARD_BUILDER_HEADER_H_

#include "genericboard.hpp"
#include "../boardtype.hpp"
#include <memory>

std::shared_ptr<GenericBoard> getBoard(BoardType type);

#endif // BOARD_BUILDER_HEADER_H_
