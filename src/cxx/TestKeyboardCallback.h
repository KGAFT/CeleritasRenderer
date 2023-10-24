#pragma once 
#include <GLFW/glfw3.h>
#include "Window/Window.hpp"

class TestKeyboardCallback : public IWindowKeyCallback{
private:
	std::vector<WindowKey> keys;
	Window* window;
public:
	TestKeyboardCallback(Window* window) : window(window) {
		keys.push_back(WindowKey{GLFW_KEY_F1, KEY_CLICKED});
	}

    void keyPressed(WindowKey *key) override {
        window->getInputSystem().setMode(MODE_RETURN_INTO_CENTER);
    }

    unsigned int getKeys(WindowKey **output) override {
        *output = keys.data();
        return keys.size();
    }

    size_t getHoldDelay() override {
        return 0;
    }
};