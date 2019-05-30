#pragma once

#ifndef GENERIC_BOARD_HEADER_H_
#define GENERIC_BOARD_HEADER_H_

/*
	This file contains:
		- Definition of a generic board class GenericBoard.
*/

#include "../boardstate.hpp"
#include "../piecetype.hpp"
#include "../pieces/piecebuilder.hpp"

#include "../profiler.hpp"

#include <memory>
#include <vector>
#include <string>

class PieceGeneric;

/**
	A generic board that any specific game's boards can derive from.
*/
class GenericBoard {
public:
	using UpgradeCallback = PieceType(*)(PieceType original, const std::vector<PieceType>& choices);
private:
	Color winner = Color::None;
	Color currentPlayer = Color::White;

	bool _canDoMove(Position fromPos, Position toPos) const;
	void _performMove(Position fromPos, Position toPos);
	void _switchColor();
	void _checkStaleOrCheckmate();
protected:
	void _clearThreat();
	void _clearThreat(BoardState& state) const;

	void _convertNulls();
	void _convertNulls(BoardState& state) const;

	void _removeShadows(Color ofColor);

	bool _isChecked(PieceStorage storage) const;

	virtual bool _checkStalemate() const;
	virtual bool _checkCheckmate() const;

	template <class UpgradeCallback>
	void doUpgrade(Position fromPos, Position toPos, UpgradeCallback callback){
		ProfileDeclare;
		static_assert(std::is_invocable_r_v<PieceType, UpgradeCallback, PieceType, const std::vector<PieceType>&>,
			"Function passed to doUpgrade "
			"must be of signature PieceType(PieceType, const std::vector<PieceType>&)."
		);

		auto& piece = state.squares[toPos.first][toPos.second];
		if (!piece.piecePtr)	return;

		auto pieceColor = piece.piecePtr->getColor();
		if (pieceColor != Color::White && pieceColor != Color::Black)	return;
		if (upgradeFieldSize <= 0)	return;

		auto upgradeField = pieceColor == Color::Black ? upgradeFieldSize - 1
			: state.height - upgradeFieldSize;

		if (toPos.first >= upgradeField) {
			auto currentType = piece.piecePtr->getType();
			auto list = piece.piecePtr->getUpgradeOptions();
			if (!list.size())	return;

			PieceType choice = PieceType::None;
			if (list.size() != 1) {
				do {
					choice = callback(currentType, list);
				} while (std::find(list.begin(), list.end(), choice) == list.end());
			}
			else {
				choice = list[0];
			}

			if (choice != currentType) {
				state.squares[toPos.first][toPos.second].piecePtr = newPieceByType(choice, pieceColor);
			}
		}
	}

	BoardState state;

	std::vector<PieceStorage> whiteGrave;
	std::vector<PieceStorage> blackGrave;

	int upgradeFieldSize = 1;
	Position selected = { -1, -1 };
public:
	GenericBoard(int boardWidth, int boardHeight, int upgradeRows = 1);
	virtual ~GenericBoard() = default;

	PieceStorage getKing() const;
	PieceStorage getKing(Color color) const;

	PieceStorage getKing(const BoardState& state) const;
	PieceStorage getKing(const BoardState& state, Color color) const;

	int getAvailableMoveCount() const;
	int getAvailableMoveCount(Color color) const;

	void addPiece(Position position, PieceType type, Color color);

	const BoardState& getState() const;
	BoardState& getState();

	void initialize(const BoardState& state);
	virtual void initialize();

	void recalculateThreat();
	void recalculateThreat(BoardState& state) const;

	const std::vector<PieceStorage>& getGraveyard(Color forColor) const;
	std::vector<PieceStorage>& getGraveyard(Color forColor);

	bool select(Position atPosition);
	void unselect();
	PieceStorage getSelected() const;
	Position getSelectedPosition() const;

	std::shared_ptr<PieceGeneric> getPiece(Position atPos) const;
	PieceStorage getPieceStorage(Position atPos) const;

	bool didMove() const;
	bool didMove(Position position) const;

	Color getPlayingColor() const;

	bool tryMove(Position fromPos, Position toPos);

	template <class UpgradeCallback>
	bool tryMove(Position fromPos, Position toPos, UpgradeCallback onUpgrade){
		ProfileDeclare;
		static_assert(std::is_invocable_r_v<PieceType, UpgradeCallback, PieceType, const std::vector<PieceType>&>,
			"Function passed to the upgrade callback version of tryMove "
			"must be of signature PieceType(PieceType, const std::vector<PieceType>&)."
		);
		auto _b = tryMove(fromPos, toPos);
		if (!_b)	return false;
		if (winner != Color::None)	return _b;

		doUpgrade(fromPos, toPos, onUpgrade);

		return true;
	}

	bool tryMove(Position toPos);

	template <class UpgradeCallback>
	bool tryMove(Position toPos, UpgradeCallback&& onUpgrade) {
		ProfileDeclare;
		static_assert(std::is_invocable_r_v<PieceType, UpgradeCallback, PieceType, const std::vector<PieceType>&>,
			"Function passed to the upgrade callback version of tryMove "
			"must be of signature PieceType(PieceType, const std::vector<PieceType>&)."
		);
		return tryMove(selected, toPos, onUpgrade);

	}

	std::vector<Position> getPossibleMoves() const;
	std::vector<Position> getPossibleMoves(Position pieceAtPos) const;

	Color getWinner() const;

	void forfeit();
};

#endif // GENERIC_BOARD_HEADER_H_
