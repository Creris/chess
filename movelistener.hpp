#pragma once

#ifndef MOVE_LISTENER_HEADER_H_
#define MOVE_LISTENER_HEADER_H_

/**
	This file contains:
		- Defintion of an interface MoveListener.
*/

#include <memory>

struct MovePropagator;

/**
	An interface that classes that want to listen for move events have to derive from.

	\sa MovePropagator()
*/
struct MoveListener {
	/**
		Executed right before a move happens.

		\param origin The origin of this call.
	*/
	virtual void preMove(MovePropagator* const origin) = 0;

	/**
		Executed right after a move happens.

		\param origin The origin of this call.
	*/
	virtual void postMove(MovePropagator* const origin) = 0;
};

#endif // MOVE_LISTENER_HEADER_H_
