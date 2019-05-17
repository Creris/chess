#include "genericboard.hpp"

#include "../pieces/piecebuilder.hpp"

void GenericBoard::_clearThreat()
{
	for (auto& row : state.squares)
		for (auto& cell : row) {
			cell.threat.fill(0);
		}
}

GenericBoard::PieceStorage GenericBoard::_getPieceStorage(Position atPos) const
{
	auto _f = std::find_if(pieces.begin(), pieces.end(), [&atPos](const PieceStorage& p) {
		return p.piecePtr->getPosition() == atPos;
						   });

	if (_f != pieces.end())
		return *_f;

	return {};
}

GenericBoard::PieceStorage GenericBoard::_getPieceStorage(Position atPos, bool isInit) const
{
	if (!isInit)
		return _getPieceStorage(atPos);

	auto _f = std::find_if(pieces.begin(), pieces.end(), [&atPos](const PieceStorage& p) {
		return p.startingPos == atPos;
	});

	if (_f != pieces.end())
		return *_f;

	return {};
}

GenericBoard::GenericBoard(int boardWidth, int boardHeight) : state{ boardWidth, boardHeight, {} }
{
	auto row = std::vector<SquareInfo>{};
	auto dummyInfo = SquareInfo{ {PieceType::None, Color::None}, {0, 0} };
	row.resize(boardWidth, dummyInfo);
	state.squares.resize(boardHeight, row);
}

void GenericBoard::addPiece(Position position, PieceInfo pieceInfo) {
	auto piece = newPieceByType(pieceInfo, position);
	if (!piece)	return;
	pieces.emplace_back(position, std::move(piece));
}

BoardState& GenericBoard::getState() {
	return state;
}

const BoardState& GenericBoard::getState() const {
	return state;
}

void GenericBoard::initialize(const BoardState& state)
{
	if (state.width == this->state.width && state.height == this->state.height)
		this->state = state;

	this->state.type = BoardType::None;

	for (size_t rowIdx = 0; rowIdx < this->state.squares.size(); ++rowIdx) {
		auto& row = this->state.squares[rowIdx];
		for (size_t colIdx = 0; colIdx < row.size(); ++colIdx) {
			auto& cell = row[colIdx];
			addPiece({ static_cast<uint8_t>(rowIdx), static_cast<uint8_t>(colIdx) }, cell.piece);
		}
	}
}

void GenericBoard::recalculateThread()
{
	_clearThreat();
	auto cToIdx = [](Color c) {
		return c == Color::White ? 0 : 1;
	};
	for (auto& piece : pieces) {
		for (auto& p : piece.piecePtr->getAllAvailableMoves(state)) {
			auto& square = state.squares[p.first][p.second];
			square.threat[cToIdx(piece.piecePtr->getColor())]++;
		}
	}
}

const std::vector<PieceInfo>& GenericBoard::getGraveyard(Color forColor) const
{
	return forColor == Color::Black ? blackGrave : whiteGrave;
}

std::shared_ptr<PieceGeneric> GenericBoard::getPiece(Position atPos) const
{
	return _getPieceStorage(atPos).piecePtr;
}

bool GenericBoard::didMove(Position position, bool isInit) const
{
	return _getPieceStorage(position, isInit).moved;
}
