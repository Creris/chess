#include "boardbuilder.hpp"

#include "chess.hpp"
#include "genericboard.hpp"

template <class T>
std::shared_ptr<GenericBoard> _getBoard() {
	static std::shared_ptr<T> board = std::make_shared<T>();
	return board;
}

std::shared_ptr<GenericBoard> getBoard(BoardType type)
{
	switch (type) {
	case BoardType::Chess:
		return _getBoard<ChessBoard>();
	}
	return nullptr;
}