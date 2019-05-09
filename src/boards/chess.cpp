#include "boardbuilder.hpp"
#include "chess.hpp"

#include "../pieces/generic.hpp"

void ChessBoard::preMove(PieceGeneric* const origin, Position targetPos)
{
	if (!origin)	return;
}

void ChessBoard::postMove(PieceGeneric* const origin, Position originPos)
{
}

inline auto _chess_builder = BuilderRegister<GenericBoard>([]() -> std::shared_ptr<GenericBoard> {
	static auto board = std::make_shared<ChessBoard>();
	return board;
	}, "Chess", "CHESS", "chess");