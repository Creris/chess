#include "piecebuilder.hpp"

#include "bishop.hpp"
#include "king.hpp"
#include "knight.hpp"
#include "pawn.hpp"
#include "queen.hpp"
#include "rook.hpp"
#include "shadowpawn.hpp"

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
