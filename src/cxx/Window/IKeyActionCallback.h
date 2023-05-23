#pragma once

class IKeyActionCallback {
public:
	virtual int* getKeys(int* amount) = 0;
	virtual void keyPressed(int key) = 0;
};