//
// Created by Danil on 13.09.2022.
//

#pragma once


#include "Camera.h"
#include "GLFW/glfw3.h"
#include "Window/Window.hpp"

class CameraMovementCallBack : public IWindowKeyCallback {
private:
    Camera *camera;
    float sensitivity;
    WindowKey keys[6]{
            {GLFW_KEY_W, KEY_PRESSED},
            {GLFW_KEY_A, KEY_PRESSED},
            {GLFW_KEY_S, KEY_PRESSED},
            {GLFW_KEY_D, KEY_PRESSED},
            {GLFW_KEY_SPACE, KEY_PRESSED},
            {GLFW_KEY_C, KEY_PRESSED}
    };
public:
    CameraMovementCallBack(Camera *camera, float sensitivity) : camera(camera),
                                                                sensitivity(sensitivity) {}


    unsigned int getKeys(WindowKey **output) override {
        *output = keys;
        return 6;
    }

    void keyPressed(WindowKey* key) override {
        float fwbAmplifier = 0;
        float leftRightAmplifier = 0;
        float upDownAmplifier = 0;
        switch (key->keyGlfwScanCode) {
            case GLFW_KEY_W:
                fwbAmplifier += sensitivity;
                break;
            case GLFW_KEY_S:
                fwbAmplifier -= sensitivity;
                break;
            case GLFW_KEY_A:
                leftRightAmplifier += sensitivity;
                break;
            case GLFW_KEY_D:
                leftRightAmplifier -= sensitivity;
                break;
            case GLFW_KEY_SPACE:
                upDownAmplifier -= sensitivity;
                break;
            case GLFW_KEY_C:
                upDownAmplifier += sensitivity;
                break;
        }
        camera->moveCam(fwbAmplifier, leftRightAmplifier, upDownAmplifier);

    }

    size_t getHoldDelay() override {
        return 0;
    }

    Camera *getCamera() const {
        return camera;
    }

    void setCamera(Camera *camera) {
        CameraMovementCallBack::camera = camera;
    }

};



