#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Monitor.h"
#include "IKeyActionCallback.h"
#include "IMouseMovementCallback.h"
#include "IWindowResizeCallback.h"


#define WINDOW_ACTION_CURSOR_MODE 0
#define WINDOW_NORMAL_CURSOR_MODE 1

#define WINDOW_MOUSE_CALLBACK_NORMAL_MODE 1
#define WINDOW_MOUSE_CALLBACK_ACTION_MODE 0
#define WINDOW_MOUSE_CALLBACK_BOTH_MODE 2

class Window {
private:
	static inline Window* windowInstance = nullptr;
	static inline bool glfwInitialized = false;

	static void resized(GLFWwindow* window, int width, int height) {
		windowInstance->width = width;
		windowInstance->height = height;
		windowInstance->sizeCallback();
	}
	

public:
	static void initializeContext() {
		if (!glfwInit()) {
			throw std::runtime_error("Failed to initialize GLFW!");
		}
		glfwInitialized = true;
	}
	static Window* createWindow(int width, int height, const char* title, Monitor* monitor) {
		if (!glfwInitialized) {
			initializeContext();
		}
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		GLFWmonitor* selectedMonitor = nullptr;
		if (monitor != nullptr) {
			selectedMonitor = monitor->getMonitor();
		}
		GLFWwindow* windowHandle = glfwCreateWindow(width, height, title, selectedMonitor, nullptr);
		if (windowHandle != nullptr) {
            glfwMakeContextCurrent(windowHandle);
            gladLoadGL();
			Window::windowInstance = new Window(width, height, title, windowHandle);
			glfwSetWindowSizeCallback(windowHandle, resized);
			return windowInstance;
		}
		else {
			const char* error = "";
			glfwGetError(&error);
			throw std::runtime_error("Failed to create window: " + std::string(error));
		}
	}

	static Window* getInstance() {
		return windowInstance;
	}

private:
	int width;
	int height;
	GLFWwindow* windowHandle;
	const char* title;
	bool isBorderless = false;

	double lastX = 0;
	double lastY = 0;
	int currentMode = WINDOW_NORMAL_CURSOR_MODE;

	std::vector<IMouseMovementCallback*> movementCallbacks;
	std::vector<IKeyActionCallback*> keyActionCallbacks;
	std::vector<IWindowResizeCallback*> resizeCallbacks;

	Window(int width, int height, const char* title, GLFWwindow* window) : title(title), width(width), height(height), windowHandle(window){}
	void sizeCallback() {
		for (const auto& element : resizeCallbacks) {
			if (element != nullptr) {
				element->resized(width, height);
			}
		}
	}

	void preCalculateMouse(double* xChangeOut, double* yChangeOut) {
        *xChangeOut = 0;
        *yChangeOut = 0;
		double cursorX = 0;
		double cursorY = 0;
		double xChange = 0;
		double yChange = 0;
		switch (currentMode) {
		case WINDOW_ACTION_CURSOR_MODE:
			glfwGetCursorPos(windowHandle, &cursorX, &cursorY);
			
			if (std::abs(cursorX - width / 2) > 0) {
				xChange = (cursorX - width / 2) / std::abs(cursorX - width / 2);
			}
			if (std::abs(cursorY - height / 2) > 0) {
				yChange = (cursorY - height / 2) / std::abs(cursorY - height / 2);
			}
			*xChangeOut = xChange;
			*yChangeOut = yChange;
			glfwSetCursorPos(windowHandle, width / 2, height / 2);
			break;
		case WINDOW_NORMAL_CURSOR_MODE:
			glfwGetCursorPos(windowHandle, &cursorX, &cursorY);
			if (cursorX != lastX || cursorY != lastY) {
				lastX = cursorX;
				lastY = cursorY;
				*xChangeOut = cursorX;
				*yChangeOut = cursorY;
			}
			break;
		}
	}
	void refreshMovementCallbacks() {
		std::vector<IMouseMovementCallback*> newMovementCallbacks;
		for (const auto element : movementCallbacks) {
			if (element != nullptr) {
				newMovementCallbacks.push_back(element);
			}
		}
		this->movementCallbacks = newMovementCallbacks;
	}

	void refreshKeyboardCallbacks() {
		std::vector<IKeyActionCallback*> newKeyCallbacks;
		for (const auto element : keyActionCallbacks) {
			if (element != nullptr) {
				newKeyCallbacks.push_back(element);
			}
		}
		this->keyActionCallbacks = newKeyCallbacks;
	}

	void checkMovementCallbacks() {
		double x, y = 0;
		preCalculateMouse(&x, &y);
		bool requiredRefresh = false;
		if (x != 0 || y != 0) {
			for (const auto& element : movementCallbacks) {
				if (element != nullptr) {
					if (element->getCallbackMode() == currentMode || element->getCallbackMode() == WINDOW_MOUSE_CALLBACK_BOTH_MODE) {
						element->mouseMoved(x, y);
					}
				}
				else {
					requiredRefresh = true;
				}
			}
		}
		if (requiredRefresh) {
			refreshMovementCallbacks();
		}
	}

	void checkKeyBoardCallbacks() {
		bool requiredRefresh = false;
		for (const auto& element : keyActionCallbacks) {
			if (element != nullptr) {
				int amount = 0;
				int* keys = element->getKeys(&amount);
				for (int i = 0; i < amount; i++) {
					if (glfwGetKey(windowHandle, keys[i])) {
						element->keyPressed(keys[i]);
					}
				}
			}
			else {
				requiredRefresh = true;
			}
		}
		if (requiredRefresh) {
			refreshKeyboardCallbacks();
		}
	}

public:
	bool needToClose() {
		return glfwWindowShouldClose(windowHandle);
	}


	void preRenderEvents() {
		checkKeyBoardCallbacks();
		checkMovementCallbacks();
	}

	void postRenderEvents() {
        glfwSwapBuffers(windowHandle);
		glfwPollEvents();
	}
	int getInputMode() {
		return currentMode;
	}

	void setMonitor(Monitor* monitor) {
		glfwSetWindowMonitor(windowHandle, monitor->getMonitor(), 0, 0, width, height, glfwGetVideoMode(monitor->getMonitor())->refreshRate);
	}

    int getWidth() const {
        return width;
    }

    int getHeight() const {
        return height;
    }

    void setSize(int width, int height) {
		this->width = width;
		this->height = height;
		glfwSetWindowSize(windowHandle, width, height);
	}
	void setTitle(const char* title) {
		this->title = title;
		glfwSetWindowTitle(windowHandle, title);
	}
	const char* getTitle() {
		return title;
	}

	void setWindowMode(int mode) {
		if (currentMode != mode) {
			switch (mode) {
				case WINDOW_NORMAL_CURSOR_MODE:
					glfwSetInputMode(windowHandle, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
					break;
				case WINDOW_ACTION_CURSOR_MODE:
					glfwSetInputMode(windowHandle, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
					break;
				default:
					throw std::runtime_error("Error: you cannot specify is not existing input mode!");
					break;
			}
			currentMode = mode;
		}
		
	}


	void registerResizeCallback(IWindowResizeCallback* resizeCallback) {
		resizeCallbacks.push_back(resizeCallback);
	}

	void registerMouseCallback(IMouseMovementCallback* mouseMovementCallback) {
		movementCallbacks.push_back(mouseMovementCallback);
	}
	void registerKeyCallback(IKeyActionCallback* keyActionCallback) {
		keyActionCallbacks.push_back(keyActionCallback);
	}
	void borderless() {
		if (!isBorderless) {
			glfwSetWindowAttrib(windowHandle, GLFW_DECORATED, GLFW_FALSE);

			isBorderless = true;
		}
		else {
			glfwSetWindowAttrib(windowHandle, GLFW_DECORATED, GLFW_TRUE);

			isBorderless = false;
		}
	}
};