#include "../../include/boards/boardbuilder.hpp"

#include "../../include/boards/chess.hpp"
#include "../../include/boards/genericboard.hpp"

#include "../../include/profiler.hpp"

template <class T>
std::shared_ptr<GenericBoard> _getBoard() {
	static std::shared_ptr<T> board = std::make_shared<T>();
	return board;
}

std::shared_ptr<GenericBoard> getBoard(BoardType type)
{
	ProfileDeclare;
	switch (type) {
	case BoardType::Chess:
		return _getBoard<ChessBoard>();
	}
	return nullptr;
}
