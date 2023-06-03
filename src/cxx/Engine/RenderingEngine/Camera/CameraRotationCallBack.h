//
// Created by Daniil on 9/13/2022.
//


#include "Camera.h"
#include "../../../Window/IMouseMovementCallback.h"
#include "../../../Window/Window.h"
#pragma once

class CameraRotationCallBack : public IMouseMovementCallback {
private:
    Camera *camera;
    float sensitivity;
public:
    CameraRotationCallBack(Camera *camera, float sensitivity) {
        this->camera = camera;
        this->sensitivity = sensitivity;
    }

    void mouseMoved(double xChange, double yChange) override {
        camera->rotateCam(sensitivity *-1*xChange, sensitivity * yChange);
    }

    Camera *getCamera() const {
        return camera;
    }
    int getCallbackMode() override{
        return WINDOW_ACTION_CURSOR_MODE;
    }
    void setCamera(Camera *camera) {
        CameraRotationCallBack::camera = camera;
    }
};



