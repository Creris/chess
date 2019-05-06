#pragma once

#ifndef MOVE_PROPAGATOR_HEADER_H_
#define MOVE_PROPAGATOR_HEADER_H_

/**
	This file contains:
		- Definition of MovePropagator, an interface that propagates move events.
*/

#include <memory>

struct MoveListener;

/**
	An interface that allows propagation of events to MoveListeners.

	Classes that want to be able to propagate these events must derive from this class.
*/
struct MovePropagator {
	/**
		Add new event listener.
		Does not prevent double addition.

		\param listener Event listener that will be notified of any moves.
	*/
	virtual void addListener(const std::shared_ptr<MoveListener>& listener) = 0;

	/**
		Remove event listener from list of listeners.
		Removes all instances of this listener from the list, does nothing if none
		are found.

		\param listener Event listener to remove from the list of listeners.
	*/
	virtual void removeListener(const std::shared_ptr<MoveListener>& listener) = 0;
	
	/** Remove all listeners. */
	virtual void clearListeners() = 0;
};

#endif // MOVE_PROPAGATOR_HEADER_H_
