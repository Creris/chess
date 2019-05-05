#pragma once

#ifndef MOVE_LISTENER_HEADER_H_
#define MOVE_LISTENER_HEADER_H_

#include <memory>

struct MovePropagator;

struct MoveListener {
	virtual void preMove(MovePropagator* const origin) = 0;
	virtual void postMove(MovePropagator* const origin) = 0;
};

#endif // MOVE_LISTENER_HEADER_H_
