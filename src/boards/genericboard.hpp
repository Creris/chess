#pragma once

#ifndef GENERIC_BOARD_HEADER_H_
#define GENERIC_BOARD_HEADER_H_

/*
	This file contains:
		- Definition of a generic board class GenericBoard.
*/

#include "../boardstate.hpp"
#include "../piecetype.hpp"

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

	virtual void doUpgrade(Position fromPos, Position toPos, UpgradeCallback callback);

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
	bool tryMove(Position fromPos, Position toPos, UpgradeCallback onUpgrade);

	bool tryMove(Position toPos);
	bool tryMove(Position toPos, UpgradeCallback onUpgrade);

	std::vector<Position> getPossibleMoves() const;
	std::vector<Position> getPossibleMoves(Position pieceAtPos) const;

	Color getWinner() const;

	void forfeit();
};

#endif // GENERIC_BOARD_HEADER_H_
