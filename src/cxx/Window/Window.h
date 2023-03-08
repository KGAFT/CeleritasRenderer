#pragma once

#include <GLFW/glfw3.h>
#include "Monitor.h"


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
	Window(int width, int height, const char* title, GLFWwindow* window) : title(title), width(width), height(height), windowHandle(window){}
	void sizeCallback() {

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
};