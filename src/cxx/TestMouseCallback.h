#pragma once

#include "Window/IMouseMovementCallback.h"
#include "Window/Window.h"
#include <iostream>
class TestMouseCallbacK : public IMouseMovementCallback {
public:
	TestMouseCallbacK() {

	}
	void mouseMoved(double xChange, double yChange) override {
		std::cout << "X: " << xChange << "Y: " << yChange << std::endl;
	}
	int getCallbackMode() override {
		return WINDOW_MOUSE_CALLBACK_BOTH_MODE;
	}
};