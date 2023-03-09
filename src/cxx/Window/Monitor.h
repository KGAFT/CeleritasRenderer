#pragma once
#include <iostream>
#include <GLFW/glfw3.h>
#include <vector>
#include <string>
class Monitor {
private:
	static inline std::vector<Monitor*> monitors;
	static void refreshMonitors() {
		for (const auto& element : monitors) {
			delete element;
		}
		monitors.clear();
		int monitorCount = 0;
		GLFWmonitor** monitors = glfwGetMonitors(&monitorCount);
		for (unsigned int i = 0; i < monitorCount; i++) {
			try {
				std::string name(glfwGetMonitorName(monitors[i]));
				int width, height = 0;
				int x, y = 0;
				glfwGetMonitorWorkarea(monitors[i],&x, &y,  &width, &height);
				Monitor::monitors.push_back(new Monitor(monitors[i], name, width-x, height-y));
			}
			catch (std::exception& e) {
				std::cerr << e.what() << std::endl;
			}
			
		}
	}
public:
	static Monitor** enumerateMonitors(int* count) {
		refreshMonitors();
		*count = monitors.size();
		return monitors.data();
	}
private:
	GLFWmonitor* monitor;
	std::string name;
	int width;
	int height;
	Monitor(GLFWmonitor* monitor, std::string name, int width, int height) : monitor(monitor), name(name), width(width), height(height){}
public:
	std::string& getName() {
		return name;
	}
	int getWidth() {
		return width;
	}
	int getHeight() {
		return height;
	}
	GLFWmonitor* getMonitor() {
		return monitor;
	}
};