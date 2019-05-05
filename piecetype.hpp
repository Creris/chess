#pragma once
#ifndef PIECE_TYPE_HEADER_H_
#define PIECE_TYPE_HEADER_H_

#include <cstdint>

/**
	Defines Piece types that the chess will know about.
*/
enum class PieceType : uint8_t {
	Pawn,			//Normal Pawn
	Knight,			//Knight
	Bishop,			//Bishop
	Rook,			//Rook
	Queen,			//Queen
	King,			//King
	ShadowPawn,		//En Passant special pawn
					//Only capturable by 
	None,			//Empty field
};

#endif	//PIECE_TYPE_HEADER_H_