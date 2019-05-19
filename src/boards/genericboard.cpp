#include "genericboard.hpp"

#include "../pieces/piecebuilder.hpp"

bool withinBounds(Position pos, int width, int height) {
	return !(pos.first < 0 || pos.second < 0 ||
			pos.first >= height || pos.second >= width);
}

void GenericBoard::_clearThreat()
{
	for (auto& row : state.squares)
		for (auto& cell : row) {
			cell.threat.fill({});
		}
}

void GenericBoard::_convertNulls()
{
	for (auto& row : state.squares) {
		for (auto& piece : row) {
			if (!piece.piecePtr)	piece.piecePtr = newPieceByType(PieceType::None);
		}
	}
}

bool GenericBoard::_checkStalemate() const
{
	return false;
}

bool GenericBoard::_checkCheckmate() const
{
	return false;
}

PieceStorage GenericBoard::_getPieceStorage(Position atPos) const
{
	if (!withinBounds(atPos, state.width, state.height))	return {};
	return state.squares[atPos.first][atPos.second];
}

PieceStorage GenericBoard::_getPieceStorage(Position atPos, bool isInit) const
{
	if (!isInit)
		return _getPieceStorage(atPos);
	
	if (!withinBounds(atPos, state.width, state.height))	return {};

	for (auto& row : state.squares) {
		for (auto& col : row) {
			if (col.startingPos == atPos)	return col;
		}
	}

	return {};
}

GenericBoard::GenericBoard(int boardWidth, int boardHeight) : state{ boardWidth, boardHeight,
																		BoardType::None, {} }
{
	auto vec = std::vector<PieceStorage>{};
	vec.resize(boardWidth);
	state.squares.resize(boardHeight, vec);
	_convertNulls();
}

void GenericBoard::addPiece(Position position, PieceType type, Color color) {
	auto piece = newPieceByType(type, color);
	if (!piece)	return;
	state.squares[position.first][position.second] = PieceStorage{ position, piece };
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

	_convertNulls();
}

void GenericBoard::initialize()
{
	_convertNulls();
}

void GenericBoard::recalculateThread()
{
	_clearThreat();
	auto cToIdx = [](Color c) {
		return c == Color::White ? 0 : 1;
	};

	for (size_t rank = 0; rank < state.squares.size(); ++rank) {
		for (size_t file = 0; file < state.squares[rank].size(); ++file) {
			auto& piece = state.squares[rank][file].piecePtr;
			for (auto& p : piece->getAllAvailableMoves({ 
														static_cast<int>(rank), 
														static_cast<int>(file)
													  }, state)) {
				auto& square = state.squares[p.first][p.second];
				square.threat[cToIdx(piece->getColor())].push_back(piece);
			}
		}
	}
}

const std::vector<PieceStorage>& GenericBoard::getGraveyard(Color forColor) const
{
	return forColor == Color::Black ? blackGrave : whiteGrave;
}

std::shared_ptr<PieceGeneric> GenericBoard::getPiece(Position atPos) const
{
	return _getPieceStorage(atPos).piecePtr;
}

bool GenericBoard::didMove(Position position, bool isInit) const
{
	return _getPieceStorage(position, isInit).piecePtr->didMove();
}

bool GenericBoard::tryMove(Position fromPos, Position toPos)
{
	auto oldSelect = selected;
	auto oldPos = selectedPosition;
	
	select(fromPos);
	bool move = tryMove(toPos);
	
	selected = oldSelect;
	selectedPosition = oldPos;

	return move;
}

bool GenericBoard::tryMove(PieceType type, Position toPos)
{
	auto oldSelect = selected;
	auto oldPos = selectedPosition;

	select(type);
	bool move = tryMove(toPos);

	selected = oldSelect;
	selectedPosition = oldPos;

	return move;
}

bool GenericBoard::tryMove(Position toPos)
{
	if (!selected)	return false;
	if (selected->getColor() != currentPlayer)	return false;

	if (selected->canMove(selectedPosition, toPos, state)) {
		selected->move(selectedPosition, toPos, state);
		unselect();
		currentPlayer = currentPlayer == Color::White ? Color::Black : Color::White;
		_convertNulls();
		return true;
	}

	return false;
}

std::string GenericBoard::getTurn(int turnNumber) const
{
	return std::string();
}

std::vector<std::string> GenericBoard::getTurns(int turnNumberStart, int turnNumberEnd) const
{
	return std::vector<std::string>();
}

std::vector<std::string> GenericBoard::getTurns() const
{
	return std::vector<std::string>();
}

inline Position _find(const BoardState& state,
										   PieceType type, Color color) {
	for (int rank = 0; rank < state.squares.size(); ++rank) {
		for (int file = 0; file < state.squares[rank].size(); ++file) {
			auto& ptr = state.squares[rank][file].piecePtr;
			if (!ptr)	continue;
			if (ptr->getType() == type && ptr->getColor() == color)	return { rank, file };
		}
	}
	return { -1, -1 };
}

void GenericBoard::select(PieceType pieceType)
{
	auto ptr = const_cast<const GenericBoard&>(*this).select(pieceType);
	if (!ptr)	return;

	selected = ptr;
	selectedPosition = _find(state, selected->getType(), selected->getColor());
}

std::shared_ptr<PieceGeneric> GenericBoard::select(PieceType pieceType) const
{
	if (pieceType == PieceType::None || currentPlayer == Color::None) {
		return nullptr;
	}

	auto& board = state.squares;
	std::shared_ptr<PieceGeneric> stored = nullptr;
	bool _break = false;

	for (size_t rank = 0; rank < board.size(); ++rank) {
		for (size_t file = 0; file < board[rank].size(); ++file) {
			auto& piece = board[rank][file].piecePtr;
			//If mismatching color, type or the piece is nullptr, continue to
			//next one
			if (!piece)								continue;
			if (piece->getColor() != currentPlayer)	continue;
			if (piece->getType() != pieceType)		continue;

			//if we already found one piece like this
			//break from the loops
			if (stored) {
				stored = nullptr;
				_break = true;
				break;
			}

			//otherwise, store it
			stored = piece;
		}

		if (_break)	break;
	}
	return stored;
}

void GenericBoard::select(Position atPos)
{
	auto ptr = const_cast<const GenericBoard&>(*this).select(atPos);
	if (!ptr)	return;
	selected = ptr;
	selectedPosition = atPos;
}

std::shared_ptr<PieceGeneric> GenericBoard::select(Position atPos) const
{
	if (!withinBounds(atPos, state.width, state.height) || currentPlayer == Color::None) {
		return nullptr;
	}

	auto& piece = state.squares[atPos.first][atPos.second].piecePtr;
	if (!piece)	return nullptr;
	auto type = piece->getType();
	if (type == PieceType::ShadowPawn || type == PieceType::None)
		return nullptr;
	if (piece->getColor() != currentPlayer)	return nullptr;

	return piece;
}

std::shared_ptr<PieceGeneric>& GenericBoard::select()
{
	return selected;
}

const std::shared_ptr<PieceGeneric>& GenericBoard::select() const
{
	return selected;
}

void GenericBoard::unselect()
{
	selected = nullptr;
	selectedPosition = { -1, -1 };
}

Color GenericBoard::getPlayingColor() const
{
	return currentPlayer;
}

std::vector<Position> GenericBoard::getAvailableMoves(Position fromPos) const
{
	return std::vector<Position>();
}

std::vector<Position> GenericBoard::getAvailableMoves(PieceType type) const
{
	std::vector<Position> vec;

	return vec;
}

std::vector<Position> GenericBoard::getAvailableMoves() const
{
	return std::vector<Position>();
}
