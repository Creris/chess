#pragma once
#ifndef BOARD_TYPE_HEADER_H_
#define BOARD_TYPE_HEADER_H_

#include <cstdint>

/**
	Defines Piece types that the chess will know about.
*/
enum class BoardType : uint8_t {
	Chess,			/**< Chess board */
	None,			/**< No board    */
};

#endif	//BOARD_TYPE_HEADER_H_