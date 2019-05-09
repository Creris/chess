#include "piecebuilder.hpp"

#include "bishop.hpp"
#include "king.hpp"
#include "knight.hpp"
#include "pawn.hpp"
#include "queen.hpp"
#include "rook.hpp"

std::shared_ptr<PieceGeneric> newPieceByType(PieceInfo fromInfo, Position position)
{
	return newPieceByType(fromInfo.type, position, fromInfo.color);
}

std::shared_ptr<PieceGeneric> newPieceByType(PieceType type, Position position, Color c)
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
		default:
			return nullptr;
		}
	};

	auto ptr = _builder(type, c);
	if (ptr) {
		ptr->setPosition(position);
	}

	return ptr;
}
