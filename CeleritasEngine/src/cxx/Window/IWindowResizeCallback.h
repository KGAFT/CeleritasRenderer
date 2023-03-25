#pragma once

class IWindowResizeCallback {
public:
	virtual void resized(int width, int height) = 0;
};