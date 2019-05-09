#pragma once

#ifndef CHESS_BOARD_HEADER_H_
#define CHESS_BOARD_HEADER_H_

#include "genericboard.hpp"

class ChessBoard : public GenericBoard {
public:
	ChessBoard() : GenericBoard(8, 8) {}
	~ChessBoard() = default;
	ChessBoard(const ChessBoard&) = default;
	ChessBoard(ChessBoard&&) noexcept = default;
	ChessBoard& operator=(const ChessBoard&) = default;
	ChessBoard& operator=(ChessBoard&&) noexcept = default;

	// Inherited via GenericBoard
	void preMove(PieceGeneric* const origin, Position targetPos) override;
	void postMove(PieceGeneric* const origin, Position originPos) override;
};

#endif // CHESS_BOARD_HEADER_H_
