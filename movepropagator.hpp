#pragma once

#ifndef MOVE_PROPAGATOR_HEADER_H_
#define MOVE_PROPAGATOR_HEADER_H_

#include <memory>

struct MoveListener;

struct MovePropagator {
	virtual void addListener(const std::shared_ptr<MoveListener>& listener) = 0;
	virtual void removeListener(const std::shared_ptr<MoveListener>& listener) = 0;
	virtual void clearListeners() = 0;
};

#endif // MOVE_PROPAGATOR_HEADER_H_
