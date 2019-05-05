#pragma once

#ifndef BOARD_STATE_HEADER_H_
#define BOARD_STATE_HEADER_H_

#include <vector>
#include <cstdint>
#include "piecetype.hpp"
#include <stdexcept>
#include <utility>

using Position = std::pair<int8_t, int8_t>;

enum class Color {
	Black,
	White,
	None,
};

struct PieceInfo {
	PieceType type;
	Color color;
};

struct BoardState {
	int width;
	int height;
	std::vector<std::vector<PieceInfo>> squares;
};

inline BoardState getEmptyBoardState() {
	return { 0, 0, {} };
}

#endif // BOARD_STATE_HEADER_H_
