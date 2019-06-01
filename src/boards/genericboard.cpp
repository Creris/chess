#include "../../include/boards/genericboard.hpp"
#include "../../include/pieces/piecebuilder.hpp"

#include "../../include/profiler.hpp"
#include "../../include/stringutil.hpp"

inline Position _findPawnFromShadow(const BoardState& state, Position shadowPos) {
	if (shadowPos.first == 5)
		return { shadowPos.first - 1, shadowPos.second };
	else if (shadowPos.first == 1)
		return { shadowPos.first + 1, shadowPos.second };
	return { -1, -1 };
}

inline PieceStorage _captureShadowPawn(BoardState& state, Position shadowPos) {
	ProfileDeclare;
	auto pawnPosition = _findPawnFromShadow(state, shadowPos);
	if (pawnPosition.first == -1 || pawnPosition.second == -1)	return {};

	auto pawnStorage = state.squares[pawnPosition.first][pawnPosition.second];
	if (!pawnStorage.piecePtr)	return {};
	if (pawnStorage.piecePtr->getType() != PieceType::Pawn)	return {};

	state.squares[pawnPosition.first][pawnPosition.second] = {};
	state.squares[pawnPosition.first][pawnPosition.second].piecePtr = newPieceByType(PieceType::None);

	return pawnStorage;
}

bool GenericBoard::withinBounds(Position pos, int width, int height) const {
	return !(pos.first < 0 || pos.second < 0 ||
			pos.first >= height || pos.second >= width);
}

void GenericBoard::_clearThreat()
{
	ProfileDeclare;
	_clearThreat(state);
}

void GenericBoard::_clearThreat(BoardState& state) const
{
	ProfileDeclare;
	for (auto& row : state.squares)
		for (auto& cell : row) {
			cell.threat.fill({});
		}
}

void GenericBoard::_convertNulls()
{
	ProfileDeclare;
	_convertNulls(state);
}

void GenericBoard::_convertNulls(BoardState& state) const
{
	ProfileDeclare;
	for (auto& row : state.squares) {
		for (auto& piece : row) {
			if (!piece.piecePtr)	piece.piecePtr = newPieceByType(PieceType::None);
		}
	}
}

bool GenericBoard::_isChecked(PieceStorage storage) const
{
	ProfileDeclare;
	auto& piece = storage.piecePtr;
	if (!piece)	return false;
	static auto colorIndex = [](Color c) { return c == Color::White ? 1 : 0; };
	return storage.threat[colorIndex(piece->getColor())];
}

void GenericBoard::_removeShadows(Color ofColor)
{
	ProfileDeclare;
	for (size_t rank = 0; rank < state.squares.size(); ++rank) {
		for (size_t file = 0; file < state.squares[rank].size(); ++file) {
			auto& piece = state.squares[rank][file].piecePtr;
			if (!piece)	continue;
			if (!(piece->getType() == PieceType::ShadowPawn && piece->getColor() == ofColor))	continue;
			state.squares[rank][file] = {};
			state.squares[rank][file].piecePtr = newPieceByType(PieceType::None);
		}
	}
}

bool GenericBoard::_checkStalemate() const
{
	return _checkStalemate(currentPlayer);
}

bool GenericBoard::_checkStalemate(Color forColor) const
{
	ProfileDeclare;
	return (!getAvailableMoveCount(forColor) && !_isChecked(getKing(forColor)));
}

bool GenericBoard::_checkCheckmate() const
{
	return _checkCheckmate(currentPlayer);
}

bool GenericBoard::_checkCheckmate(Color forColor) const
{
	ProfileDeclare;
	return (!getAvailableMoveCount(forColor) && _isChecked(getKing(forColor)));
}

std::string GenericBoard::parseTurnToString(Position from, PieceType fromType, Color fromColor,
											Position to, PieceType toType, Color toColor,
											PieceType upgraded) const
{
	ProfileDeclare;

	using namespace std::string_literals;

	if (!withinBounds(from, state.width, state.height) ||
		!withinBounds(to, state.width, state.height))
		return "";

	static auto outputChar = [](PieceType type) {
		switch (type) {
			case PieceType::King:
				return 'K';
			case PieceType::Queen:
				return 'Q';
			case PieceType::Bishop:
				return 'B';
			case PieceType::Knight:
				return 'N';
			case PieceType::Rook:
				return 'R';
		}
		return '\0';
	};


	Position diff = { from.first - to.first, from.second - to.second };

	std::string fromStr = positionToString(from);
	std::string toStr = positionToString(to);
	std::string suffix = "";

	auto enemyColor = fromColor == Color::White ? Color::Black : Color::White;

	//Check castling
	if (fromType == PieceType::King && !diff.first && std::abs(diff.second) == 2) {
		//Queenside
		if (diff.second > 0)	return "0-0-0";
		//Kingside
		else					return "0-0";
	}

	std::string captureSymbol = "";
	//A capture is indicated with x between the squares we move from and to.
	if (toType != PieceType::None
		|| (toType == PieceType::ShadowPawn && fromType == PieceType::Pawn)) {
		captureSymbol = 'x';
	}


	char pieceCode = outputChar(fromType);
	if (fromType == PieceType::Pawn) {
		//Pawn only needs file it moved from
		fromStr = toType == PieceType::None ? '\0' : fromStr[0];
		if (upgraded != PieceType::None)
			suffix = outputChar(upgraded);
	}

	//Check en passant
	if (toType == PieceType::ShadowPawn && fromType == PieceType::Pawn) {
		//En Passant capture, file from + x + destination square + e.p. code for en passant
		suffix += "e.p.";
	}

	auto isChecked = _isChecked(getKing(enemyColor));
	auto available = getAvailableMoveCount(enemyColor);

	if (isChecked) {
		suffix += available > 0 ? "+" : "#";
	}

	//Classic syntax, Piece code + source square + capture symbol, if any + destination square.
	return pieceCode + fromStr + captureSymbol + toStr + suffix;
}

void GenericBoard::writeDownMove(Position from, PieceType fromType, Color fromColor,
								 Position to, PieceType toType, Color toColor,
								 PieceType upgraded)
{
	using namespace std::string_literals;

	auto parsed = strip(parseTurnToString(from, fromType, fromColor, to, toType, toColor, upgraded), "\0 "s);
	if (currentPlayer == Color::Black) {
		turnStrings.back().second = parsed;
		turnStrings.emplace_back();
		turnNumber++;
	}
	else {
		turnStrings.back().first = parsed;
	}
}

void GenericBoard::writeDownForfeit()
{
	if (currentPlayer == Color::Black) {
		turnStrings.back().second = "1-0";
		turnStrings.emplace_back();
		turnNumber++;
	}
	else {
		turnStrings.back().first = "0-1";
	}
}

void GenericBoard::writeDownPat()
{
	turnStrings.emplace_back("1/2 -", "1/2");
}

PieceStorage GenericBoard::getKing() const
{
	ProfileDeclare;
	return getKing(state, getPlayingColor());
}

PieceStorage GenericBoard::getKing(Color color) const
{
	ProfileDeclare;
	return getKing(state, color);
}

PieceStorage GenericBoard::getKing(const BoardState& state) const
{
	ProfileDeclare;
	return getKing(state, getPlayingColor());
}

PieceStorage GenericBoard::getKing(const BoardState& state, Color color) const
{
	ProfileDeclare;
	for (size_t rank = 0; rank < state.squares.size(); ++rank) {
		for (size_t file = 0; file < state.squares[rank].size(); ++file) {
			auto& piece = state.squares[rank][file].piecePtr;
			if (!piece)	continue;
			if (piece->getType() == PieceType::King && piece->getColor() == color)
				return state.squares[rank][file];
		}
	}

	return {};
}

int GenericBoard::getAvailableMoveCount() const
{
	ProfileDeclare;
	return getAvailableMoveCount(getPlayingColor());
}

int GenericBoard::getAvailableMoveCount(Color color) const
{
	ProfileDeclare;
	int counter = 0;
	for (size_t rank = 0; rank < state.squares.size(); ++rank) {
		for (size_t file = 0; file < state.squares[rank].size(); ++file) {
			auto& piece = state.squares[rank][file].piecePtr;
			if (!piece)						continue;
			if (piece->getColor() != color)	continue;
			Position pos = { static_cast<int>(rank), static_cast<int>(file) };
			counter += getPossibleMoves(pos).size();
		}
	}
	return counter;
}

GenericBoard::GenericBoard(int boardWidth, int boardHeight,
						   int upgradeSize) : state{ boardWidth, boardHeight,
													BoardType::None, {} },
												upgradeFieldSize(upgradeSize)
{
	ProfileDeclare;
	auto vec = std::vector<PieceStorage>{};
	vec.resize(boardWidth);
	state.squares.resize(boardHeight, vec);
	_convertNulls();
}

void GenericBoard::addPiece(Position position, PieceType type, Color color) {
	ProfileDeclare;
	auto oldPiece = getPiece(position);
	if (!oldPiece || oldPiece->getType() != PieceType::None)	return;
	auto piece = newPieceByType(type, color);
	if (!piece)	return;
	state.squares[position.first][position.second] = PieceStorage{ position, piece };
}

void GenericBoard::addPiece(const char* strPos, PieceType type, Color color)
{
	auto pos = stringToPosition(strPos);
	if (withinBounds(pos, state.width, state.height))
		addPiece(pos, type, color);
}

BoardState& GenericBoard::getState() {
	return state;
}

const BoardState& GenericBoard::getState() const {
	return state;
}

void GenericBoard::initialize(const BoardState& state)
{
	ProfileDeclare;
	if (state.width == this->state.width && state.height == this->state.height)
		this->state = state;

	initialize();
}

void GenericBoard::initialize()
{
	ProfileDeclare;
	blackGrave.clear();
	whiteGrave.clear();
	selected = { -1, -1 };
	winner = Color::None;
	currentPlayer = Color::White;
	turnStrings.clear();
	turnStrings.shrink_to_fit();
	turnStrings.resize(1);
	turnNumber = 1;

	_convertNulls();
	recalculateThreat();
}

void GenericBoard::recalculateThreat()
{
	recalculateThreat(state);
}

void GenericBoard::recalculateThreat(BoardState& state) const
{
	ProfileDeclare;
	_clearThreat(state);
	auto cToIdx = [](Color c) {
		return c == Color::White ? 0 : 1;
	};

	for (size_t rank = 0; rank < state.squares.size(); ++rank) {
		for (size_t file = 0; file < state.squares[rank].size(); ++file) {
			auto& piece = state.squares[rank][file].piecePtr;
			if (!piece)	continue;
			auto type = piece->getType();
			if (type == PieceType::None || type == PieceType::ShadowPawn)
				continue;

			for (auto& p : piece->getAllThreateningMoves({ 
														static_cast<int>(rank), 
														static_cast<int>(file)
													  }, state)) {
				auto& square = state.squares[p.first][p.second];
				square.threat[cToIdx(piece->getColor())]++;
			}
		}
	}
}

const std::vector<PieceStorage>& GenericBoard::getGraveyard(Color forColor) const
{
	return forColor == Color::Black ? blackGrave : whiteGrave;
}

std::vector<PieceStorage>& GenericBoard::getGraveyard(Color forColor)
{
	return forColor == Color::Black ? blackGrave : whiteGrave;
}

bool GenericBoard::select(Position atPosition)
{
	ProfileDeclare;
	if (winner != Color::None)	return false;

	if (!withinBounds(atPosition, state.width, state.height))
		return false;

	auto piece = getPieceStorage(atPosition);
	if (!piece.piecePtr)	return false;
	auto type = piece.piecePtr->getType();
	if (type == PieceType::None || type == PieceType::ShadowPawn)
		return false;

	if (currentPlayer != piece.piecePtr->getColor())	return false;

	selected = atPosition;
	return true;
}

void GenericBoard::unselect()
{
	selected = { -1, -1 };
}

PieceStorage GenericBoard::getSelected() const
{
	if (selected.first != -1 || selected.second != -1)
		return state.squares[selected.first][selected.second];
	return {};
}

Position GenericBoard::getSelectedPosition() const
{
	return selected;
}

std::shared_ptr<PieceGeneric> GenericBoard::getPiece(Position atPos) const
{
	if (!withinBounds(atPos, state.width, state.height))
		return {};
	return state.squares[atPos.first][atPos.second].piecePtr;
}

PieceStorage GenericBoard::getPieceStorage(Position atPos) const
{
	if (!withinBounds(atPos, state.width, state.height))
		return {};
	return state.squares[atPos.first][atPos.second];
}

bool GenericBoard::didMove() const
{
	return didMove(selected);
}

bool GenericBoard::didMove(Position position) const
{
	if (!withinBounds(position, state.width, state.height))
		return false;

	return getPieceStorage(position).didMove;
}

Color GenericBoard::getPlayingColor() const
{
	return currentPlayer;
}

bool GenericBoard::tryMove(Position fromPos, Position toPos)
{
	ProfileDeclare;
	if (winner != Color::None)	return false;

	if (!withinBounds(fromPos, state.width, state.height) || 
			!withinBounds(toPos, state.width, state.height))
		return false;

	auto& selected = state.squares[fromPos.first][fromPos.second];
	if (!selected.piecePtr)	return false;
	if (selected.piecePtr->getColor() != currentPlayer)	return false;

	if (_canDoMove(fromPos, toPos)) {
		auto& toSquare = state.squares[toPos.first][toPos.second].piecePtr;
		
		auto fromType = selected.piecePtr->getType();
		auto fromColor = selected.piecePtr->getColor();

		auto toType = toSquare->getType();
		auto toColor = toSquare->getColor();

		_performMove(fromPos, toPos);

		recalculateThreat();

		writeDownMove(fromPos, fromType, fromColor, toPos, toType, toColor, PieceType::None);
		_switchColor();
		_removeShadows(currentPlayer);
		_checkStaleOrCheckmate();
		state.squares[toPos.first][toPos.second].didMove = true;
		return true;
	}

	return false;
}

bool GenericBoard::tryMove(Position toPos)
{
	return tryMove(selected, toPos);
}

std::vector<Position> GenericBoard::getPossibleMoves() const
{
	return getPossibleMoves(selected);
}

std::vector<Position> GenericBoard::getPossibleMoves(Position pieceAtPos) const
{
	ProfileDeclare;
	if (!withinBounds(pieceAtPos, state.width, state.height))
		return {};
	
	auto& piece = state.squares[pieceAtPos.first][pieceAtPos.second];
	if (!piece.piecePtr)	return {};

	std::vector<Position> filtered;

	for (auto& pos : piece.piecePtr->getAllAvailableMoves(pieceAtPos, state)) {
		if (_canDoMove(pieceAtPos, pos))
			filtered.push_back(pos);
	}

	return filtered;
}

Color GenericBoard::getWinner() const
{
	return winner;
}

void GenericBoard::forfeit()
{
	if (winner != Color::None)	return;
	winner = currentPlayer == Color::White ? Color::Black : Color::White;
	writeDownForfeit();
}

int GenericBoard::getTurn() const
{
	return turnNumber;
}

std::pair<std::string, std::string> GenericBoard::getTurnInfo() const
{
	if (!turnStrings.size())	return {};
	return turnStrings.back();
}

std::pair<std::string, std::string> GenericBoard::getTurnInfo(int turn) const
{
	if (turn < 1 || turn > turnStrings.size())	return {};
	return turnStrings[turn - 1];
}

std::vector<std::pair<std::string, std::string>> GenericBoard::getTurnInfo(int turn, int until) const
{
	if (turn < 1 || turn > turnStrings.size())	return {};
	if (until < 1 || until < turn)	return {};

	std::vector<std::pair<std::string, std::string>> v;
	
	until = std::min(turnStrings.size(), static_cast<size_t>(until));

	for (turn = turn - 1; turn <= until - 1; ++turn)
		v.push_back(turnStrings[turn]);

	return v;
}

bool GenericBoard::_canDoMove(Position fromPos, Position toPos) const
{
	ProfileDeclare;
	if (!withinBounds(fromPos, state.width, state.height) ||
			!withinBounds(toPos, state.width, state.height))
		return false;

	auto fakeState = state;
	auto& piece = fakeState.squares[fromPos.first][fromPos.second];
	auto thisPlayer = piece.piecePtr->getColor();
	auto enemyPlayer = thisPlayer == Color::White ? 1 : 0;

	if (!piece.piecePtr)	return false;
	auto moved = piece.piecePtr->move(fromPos, toPos, fakeState);
	if (!moved.first)	return false;


	recalculateThreat(fakeState);
	if (getKing(fakeState, thisPlayer).threat[enemyPlayer])
		return false;

	return true;
}

void GenericBoard::_performMove(Position fromPos, Position toPos)
{
	ProfileDeclare;
	if (!withinBounds(fromPos, state.width, state.height) ||
			!withinBounds(toPos, state.width, state.height))
		return;

	//get reference to current piece
	auto& piece = state.squares[fromPos.first][fromPos.second];

	//if it is nullptr, or the color isnt currently playing player's color
	if (!piece.piecePtr)	return;
	if (piece.piecePtr->getColor() != currentPlayer)	return;

	//Attempt to move, the move itself will return whether it was success
	//or not so we dont have to double check possibility.
	auto moved = piece.piecePtr->move(fromPos, toPos, state);
	
	//if first is false, the move fas failure
	if (!moved.first)	return;
	
	//if the piece we moved on top of is not nullptr and its None
	//store it in its player's graveyard
	if (moved.second.piecePtr && moved.second.piecePtr->getType() != PieceType::None)
		getGraveyard(moved.second.piecePtr->getColor()).push_back(moved.second);
}

void GenericBoard::_switchColor()
{
	currentPlayer = currentPlayer == Color::White ? Color::Black : Color::White;
}

void GenericBoard::_checkStaleOrCheckmate()
{
	ProfileDeclare;
	auto movesAvailable = getAvailableMoveCount();
	bool checked = _isChecked(getKing());

	if (!movesAvailable && !checked) {
		winner = Color::Pat;
		currentPlayer = Color::None;
	}
	else if (!movesAvailable && checked) {
		_switchColor();
		winner = currentPlayer;
		currentPlayer = Color::None;
	}
}