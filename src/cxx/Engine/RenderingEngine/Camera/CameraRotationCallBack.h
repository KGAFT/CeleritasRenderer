//
// Created by Daniil on 9/13/2022.
//


#include "Camera.h"
#include "Window/Window.hpp"

#pragma once

class CameraRotationCallBack : public IWindowMouseMovementCallback {
private:
    Camera *camera;
    float sensitivity;
public:
    CameraRotationCallBack(Camera *camera, float sensitivity) {
        this->camera = camera;
        this->sensitivity = sensitivity;
    }

    void moved(glm::vec2 oldPos, glm::vec2 newPos) override {
        camera->rotateCam(sensitivity * -1 * (newPos.x-oldPos.x), sensitivity * (newPos.y-oldPos.y));
    }

    WindowInputMode getRequireWorkMode() override {
        return MODE_FREE_CURSOR;
    }


    Camera *getCamera() const {
        return camera;
    }

    void setCamera(Camera *camera) {
        CameraRotationCallBack::camera = camera;
    }
};



