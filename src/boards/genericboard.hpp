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
	Color currentPlayer = Color::White;
	std::shared_ptr<PieceGeneric> selected = nullptr;
	Position selectedPosition;
protected:
	void _clearThreat();
	void _convertNulls();

	virtual bool _checkStalemate() const;
	virtual bool _checkCheckmate() const;

	//int getAvailableMoveCount(Color color) const;

	/**
		Contains the state of the board.

		Also contains information about pieces on the board in squares members.
	*/
	BoardState state;

	/**
		White's graveyard.
	*/
	std::vector<PieceStorage> whiteGrave;

	/**
		Black's graveyard.
	*/
	std::vector<PieceStorage> blackGrave;

	PieceStorage _getPieceStorage(Position atPos) const;
	PieceStorage _getPieceStorage(Position atPos, bool isInit) const;
public:
	/**
		Construct a new instance of GenericBoard.

		The board will contain empty board state that has specified size.

		\param boardWidth Width of the board.
		\param boardHeight Height of the board.
	*/
	GenericBoard(int boardWidth, int boardHeight);

	/**
		Default virtual destructor, so we can run destructors of derived classes.
	*/
	virtual ~GenericBoard() = default;

	/**
		Adds new piece to the board at specified position provided
		given info.

		The threat member of PieceInfo is ingored.

		\sa PieceInfo()
		\param position The position to add the piece on the board into.
		\param pieceInfo Piece info about the piece to put onto the board.
	*/
	void addPiece(Position position, PieceType type, Color color);

	/**
		Gets the state of the board.

		\return State of the board.
	*/
	const BoardState& getState() const;

	/**
		Gets the state of the board.

		\return State of the board.
	*/
	BoardState& getState();

	/**
		Initialize the board to given state.

		The state is copied.

		\param state State to initialize this board with.
	*/
	void initialize(const BoardState& state);

	/**
		Initialize the board to its innate, basic state.
	*/
	virtual void initialize();

	/**
		Recalculate all threats of all pieces on the board.
	*/
	void recalculateThread();

	/**
		Get a list of pieces in given player's graveyard.

		\param forColor Color of the player to retreive the graveyard for.
		\return Vector of PieceInfo of graveyarded pieces.

		\sa Color()
		\sa PieceInfo()
	*/
	const std::vector<PieceStorage>& getGraveyard(Color forColor) const;

	/**
		Get a pointer to a piece at given position.

		\param atPos The position the piece should be standing at.
		\return Pointer to a piece standing at this position.
				If none, returns nullptr.

		\sa Position()
		\sa PieceGeneric()
	*/
	std::shared_ptr<PieceGeneric> getPiece(Position atPos) const;

	/**
		Get whether a piece at a given position moved at all.

		\param position Position to look for.
		\param isInit Whether the position is initial or current.
		
		\return Whether a piece found in that position has ever moved.
	*/
	bool didMove(Position position, bool isInit = false) const;

	/**
		Tries to move a piece from position to position.

		If the piece at fromPos is not of the color of the player thats moving,
		does nothing.

		If there is no piece at fromPos, does nothing.
		
		If the piece at fromPos can't move towards toPos, does nothing.

		If the piece moved, the target piece, if any, is moved to graveyard,
		converts all null pieces to instances of Generic piece and switches
		the playing player's color.

		\param fromPos Position to try to move the piece under.
		\param toPos Position to move the piece at fromPos towards.
		\return Whether the piece moved or not.
	*/
	virtual bool tryMove(Position fromPos, Position toPos);

	/**
		Tries to find a piece of a certain type towards given position.

		If there are more pieces of given type, does nothing.

		If the identified piece is not of the color of the player thats moving,
		does nothing.

		If there is no identified piece, does nothing.
		
		If the identified piece can't move towards toPos, does nothing.

		If the piece moved, the target piece, if any, is moved to graveyard,
		converts all null pieces to instances of Generic piece and switches
		the playing player's color.

		\param type A type of the piece to attempt to move.
		\param toPos Position to move the piece at fromPos towards.
		\return Whether the piece moved or not.
	*/
	virtual bool tryMove(PieceType type, Position toPos);

	/**
		Tries to move a currently selected piece.

		If that piece is not of the color of the player thats moving,
		does nothing.

		If there is no selected piece, does nothing.

		If the selected piece can't move towards toPos, does nothing.

		If the piece moved, the target piece, if any, is moved to graveyard,
		converts all null pieces to instances of Generic piece and switches
		the playing player's color.

		\param toPos Position to move the piece at fromPos towards.
		\return Whether the piece moved or not.
	*/
	virtual bool tryMove(Position toPos);
	
	std::string getTurn(int turnNumber) const;

	std::vector<std::string> getTurns(int turnNumberStart, int turnNumberEnd) const;

	std::vector<std::string> getTurns() const;

	
	void select(PieceType pieceType);
	std::shared_ptr<PieceGeneric> select(PieceType pieceType) const;
	
	void select(Position atPos);
	std::shared_ptr<PieceGeneric> select(Position atPos) const;

	std::shared_ptr<PieceGeneric>& select();

	const std::shared_ptr<PieceGeneric>& select() const;

	/**
		Removes selection from this board.
	*/
	void unselect();

	/**
		Retreives the current playing player's color.

		\return Color of the player that is moving next.
	*/
	Color getPlayingColor() const;

	std::vector<Position> getAvailableMoves(Position fromPos) const;
	std::vector<Position> getAvailableMoves(PieceType type) const;
	std::vector<Position> getAvailableMoves() const;
};

#endif // GENERIC_BOARD_HEADER_H_
