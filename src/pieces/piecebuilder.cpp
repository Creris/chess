#include "../../include/pieces/piecebuilder.hpp"

#include "../../include/pieces/bishop.hpp"
#include "../../include/pieces/king.hpp"
#include "../../include/pieces/knight.hpp"
#include "../../include/pieces/pawn.hpp"
#include "../../include/pieces/queen.hpp"
#include "../../include/pieces/rook.hpp"
#include "../../include/pieces/shadowpawn.hpp"

std::shared_ptr<PieceGeneric> newPieceByType(PieceType type, Color c)
{
	auto _builder = [](PieceType type, Color c) -> std::shared_ptr<PieceGeneric> {
		switch (type) {
		case PieceType::Bishop:
			return std::make_shared<PieceBishop>(c);
		case PieceType::King:
			return std::make_shared<PieceKing>(c);
		case PieceType::Pawn:
			return std::make_shared<PiecePawn>(c);
		case PieceType::Queen:
			return std::make_shared<PieceQueen>(c);
		case PieceType::Rook:
			return std::make_shared<PieceRook>(c);
		case PieceType::Knight:
			return std::make_shared<PieceKnight>(c);
		case PieceType::None:
			return std::make_shared<PieceGeneric>(c);
		case PieceType::ShadowPawn:
			return std::make_shared<PieceShadowPawn>(c);
		default:
			return nullptr;
		}
	};

	return _builder(type, c);
}
