#pragma once

class IMouseMovementCallback {
public:
	virtual void mouseMoved(double xChange, double yChange) = 0;
	virtual int getCallbackMode() = 0;
};