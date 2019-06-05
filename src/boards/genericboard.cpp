#include "../../include/boards/genericboard.hpp"
#include "../../include/pieces/piecebuilder.hpp"

#include "../../include/profiler.hpp"
#include "../../include/stringutil.hpp"
#include "../../include/ui/conactions.hpp"

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
	return storage.threat[colorIndex(piece->getColor())].size();
}

std::string GenericBoard::_stateToString(const BoardState& state)
{
	std::string s;
	
	s += std::to_string(getPossibleMoves(state, getKingPos(state, Color::White)).size()) + "\n";
	s += std::to_string(getPossibleMoves(state, getKingPos(state, Color::Black)).size()) + "\n";

	static auto colorize = [](Color c, char ch) {
		return c == Color::Black ? std::tolower(ch) : ch;
	};

	for (auto& row : state.squares) {
		for (auto& storage : row) {
			s += colorize(storage.piecePtr->getColor(), typeToCharRaw(storage.piecePtr->getType()));
		}
		s += "\n";
	}

	return s;
}

bool GenericBoard::_checkRepetition()
{
	static auto colorToIdx = [](Color c) {
		return c == Color::White ? 0 : 1;
	};

	std::string stringed = _stateToString(state);
	auto& conf = this->configurations[colorToIdx(currentPlayer)];
	return ++conf[stringed] >= 3;
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

std::vector<Position> _attacksOfType(PieceType type,
									 const threat_t& input) {
	std::vector<Position> poss;

	for (auto& v : input) {
		if (v.second == type)
			poss.push_back(v.first);
	}

	return poss;
}

bool _multipleFromRank(int8_t rank, const std::vector<Position>& poss) {
	int occurence = 0;
	for (auto& pos : poss) {
		if (rank == pos.first) {
			if (occurence)	return true;
			occurence++;
		}
	}

	return false;
}

bool _multipleFromFile(int8_t file, const std::vector<Position>& poss) {
	int occurence = 0;
	for (auto& pos : poss) {
		if (file == pos.second) {
			if (occurence)	return true;
			occurence++;
		}
	}

	return false;
}

std::string GenericBoard::parseTurnToString(Position from, PieceType fromType,
											Color fromColor, Position to,
											PieceType toType, Color toColor,
											PieceType upgraded,
											const std::array<threat_t, 2> & threats) const
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

	auto attIdx = [](Color c) { return c == Color::White ? 0 : 1; };
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

	auto poss = _attacksOfType(fromType, threats[attIdx(fromColor)]);
	auto multiFile = _multipleFromFile(from.second, poss);
	auto multiRank = _multipleFromRank(from.first, poss);

	if (poss.size() == 1)
		fromStr = "";
	else if (!multiFile && !multiRank) {
		fromStr = fromStr[0];
	}
	else if (!multiFile && multiRank) {
		fromStr = fromStr[0];
	}
	else if (multiFile && multiRank) {
		//fromStr = fromStr;
	}
	else {
		fromStr = fromStr[1];
	}

	//Classic syntax, Piece code + source square + capture symbol, if any + destination square.
	return pieceCode + fromStr + captureSymbol + toStr + suffix;
}

void GenericBoard::writeDownMove(Position from, PieceType fromType,
								 Color fromColor, Position to,
								 PieceType toType, Color toColor,
								 PieceType upgraded,
								 const std::array<threat_t, 2> & threats)
{
	using namespace std::string_literals;

	auto parsed = strip(parseTurnToString(from, fromType, fromColor,
										  to, toType, toColor,
										  upgraded, threats), "\0 "s);
	if (currentPlayer == Color::Black) {
		turnStrings.back().second = parsed;
		turnStrings.emplace_back();
		++turnNumber;
	}
	else {
		turnStrings.back().first = parsed;
	}

	moveNumber++;
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

	moveNumber++;
}

void GenericBoard::writeDownPat()
{
	using namespace std::string_literals;

	if (turnStrings.back().first == "" && turnStrings.back().second == "")
		turnStrings.back() = std::make_pair("1/2 -"s, "1/2"s);
	else {
		turnStrings.emplace_back("1/2 -", "1/2");
		turnNumber++;
	}

	moveNumber++;
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

Position GenericBoard::getKingPos() const
{
	ProfileDeclare;
	return getKingPos(state, getPlayingColor());
}

Position GenericBoard::getKingPos(Color color) const
{
	ProfileDeclare;
	return getKingPos(state, color);
}

Position GenericBoard::getKingPos(const BoardState& state) const
{
	ProfileDeclare;
	return getKingPos(state, getPlayingColor());
}

Position GenericBoard::getKingPos(const BoardState& state, Color color) const
{
	ProfileDeclare;
	for (size_t rank = 0; rank < state.squares.size(); ++rank) {
		for (size_t file = 0; file < state.squares[rank].size(); ++file) {
			auto& piece = state.squares[rank][file].piecePtr;
			if (!piece)	continue;
			if (piece->getType() == PieceType::King && piece->getColor() == color)
				return { static_cast<int8_t>(rank), static_cast<int8_t>(file) };
		}
	}

	return { -1, -1 };
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
	lastProgress = 1;
	moveNumber = 1;

	configurations.fill({});
	_checkRepetition();

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

			Position pos = { static_cast<int>(rank), static_cast<int>(file) };

			for (auto& p : piece->getAllThreateningMoves(pos, state)) {
				auto& square = state.squares[p.first][p.second];
				square.threat[cToIdx(piece->getColor())].emplace_back(pos, type);
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

	auto& piece = getPieceStorage(atPosition);
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

const PieceStorage& GenericBoard::getPieceStorage(Position atPos) const
{
	if (!withinBounds(atPos, state.width, state.height))
		throw std::out_of_range("Invalid position");
	return state.squares[atPos.first][atPos.second];
}

PieceStorage& GenericBoard::getPieceStorage(Position atPos) {
	if (!withinBounds(atPos, state.width, state.height))
		throw std::out_of_range("Invalid position");
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
		auto threats = getPieceStorage(toPos).threat;

		_performMove(fromPos, toPos);

		recalculateThreat();

		writeDownMove(fromPos, fromType, fromColor, toPos, toType, toColor, PieceType::None, threats);
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
	return getPossibleMoves(state, selected);
}

std::vector<Position> GenericBoard::getPossibleMoves(Position pieceAtPos) const
{
	return getPossibleMoves(state, pieceAtPos);
}

std::vector<Position> GenericBoard::getPossibleMoves(const BoardState& state) const
{
	return getPossibleMoves(state, selected);
}

std::vector<Position> GenericBoard::getPossibleMoves(const BoardState& state, Position pieceAtPos) const
{
	ProfileDeclare;
	if (!withinBounds(pieceAtPos, state.width, state.height))
		return {};

	auto& piece = state.squares[pieceAtPos.first][pieceAtPos.second];
	if (!piece.piecePtr)	return {};

	std::vector<Position> filtered;

	for (auto& pos : piece.piecePtr->getAllAvailableMoves(pieceAtPos, state)) {
		if (_canDoMove(state, pieceAtPos, pos))
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
	return _canDoMove(state, fromPos, toPos);
}

void GenericBoard::_performMove(Position fromPos, Position toPos)
{
	_performMove(state, fromPos, toPos);
}

bool GenericBoard::_canDoMove(const BoardState& state, Position fromPos, Position toPos) const
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
	if (getKing(fakeState, thisPlayer).threat[enemyPlayer].size())
		return false;

	return true;
}

void GenericBoard::_performMove(BoardState& state, Position fromPos, Position toPos)
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

	//if the piece we moved on top of is not nullptr and its not None
	//store it in its player's graveyard
	if (moved.second.piecePtr && moved.second.piecePtr->getType() != PieceType::None) {
		getGraveyard(moved.second.piecePtr->getColor()).push_back(moved.second);

		//we captured, update last progress number
		lastProgress = moveNumber;
	}
	//If we moved pawn even if we didnt capture, its progress
	else if (piece.piecePtr->getType() == PieceType::Pawn) {
		lastProgress = moveNumber;
	}
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

	if ((!movesAvailable && !checked) || moveNumber + 50 <= lastProgress || _checkRepetition()) {
		winner = Color::Pat;
		writeDownPat();
		currentPlayer = Color::None;
	}
	else if (!movesAvailable && checked) {
		_switchColor();
		winner = currentPlayer;
		currentPlayer = Color::None;
	}
}