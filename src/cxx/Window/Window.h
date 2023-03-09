#pragma once

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
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		GLFWmonitor* selectedMonitor = nullptr;
		if (monitor != nullptr) {
			selectedMonitor = monitor->getMonitor();
		}
		GLFWwindow* windowHandle = glfwCreateWindow(width, height, title, selectedMonitor, nullptr);
		if (windowHandle != nullptr) {
            glfwMakeContextCurrent(windowHandle);
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
public:
	bool needToClose() {
		return glfwWindowShouldClose(windowHandle);
	}
	void postRenderEvents() {
		glfwPollEvents();
	}

	void setMonitor(Monitor* monitor) {
		glfwSetWindowMonitor(windowHandle, monitor->getMonitor(), 0, 0, width, height, glfwGetVideoMode(monitor->getMonitor())->refreshRate);
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