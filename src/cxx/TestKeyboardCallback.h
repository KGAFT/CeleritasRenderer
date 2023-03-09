#pragma once 
#include <GLFW/glfw3.h>
#include "Window/Window.h"
#include "Window/IKeyActionCallback.h"

class TestKeyboardCallback : public IKeyActionCallback{
private:
	std::vector<int> keys;
	Window* window;
public:
	TestKeyboardCallback(Window* window) : window(window) {
		keys.push_back(GLFW_KEY_F1);
		keys.push_back(GLFW_KEY_DELETE);
	}

	int* getKeys(int* amount) override {
		*amount = keys.size();
		return keys.data();
	}
	void keyPressed(int key) {
		switch (key) {
		case GLFW_KEY_F1:
			window->setWindowMode(!window->getInputMode());
			break;
		case GLFW_KEY_DELETE:
			delete this;
		}
	}
};