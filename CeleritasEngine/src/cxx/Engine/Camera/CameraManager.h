//
// Created by Daniil on 26.12.2022.
//

#pragma once

#include "Camera.h"
#include "CameraMovementCallBack.h"
#include "CameraRotationCallBack.h"
#include "../../Window/Window.h"
#include "CameraResizeCallBack.h"

struct PushConstantData{
    glm::mat4 viewMatrix;
    glm::mat4 worldMatrix;
    glm::vec3 cameraPosition;
};

class CameraManager{
private:
    static inline CameraManager* cameraManagerInstance = nullptr;

private:
    float fov = 90.0f;
    Camera* currentCamera = new Camera(glm::vec3(0, 0, 2.0f), Window::getInstance()->getWidth(), Window::getInstance()->getHeight());
    CameraMovementCallBack* currentCameraMovementCallback = new CameraMovementCallBack(currentCamera, 0.02f);
    CameraRotationCallBack* currentCameraRotationCallBack = new CameraRotationCallBack(currentCamera, 0.5f);
    CameraResizeCallBack* cameraResizeCallBack = new CameraResizeCallBack(currentCamera);
    PushConstantData* data;

public:
    CameraManager(PushConstantData* data){
        this->data = data;
        Window::getInstance()->registerResizeCallback(cameraResizeCallBack);
        Window::getInstance()->registerKeyCallback(currentCameraMovementCallback);
        Window::getInstance()->registerMouseCallback(currentCameraRotationCallBack);
    }
    void update(){
        glm::mat4 camMat = currentCamera->getCameraMatrix(fov, 0.001f, 100.0f);
        data->viewMatrix = camMat;
        data->worldMatrix = glm::mat4(1.0f);
        data->cameraPosition = currentCamera->getPosition();
    }
    void setCurrentCamera(Camera* currentCamera){
        this->currentCamera = currentCamera;
        currentCameraRotationCallBack->setCamera(this->currentCamera);
        currentCameraMovementCallback->setCamera(this->currentCamera);
        cameraResizeCallBack->setCurrentCamera(this->currentCamera);
    }

    Camera *getCurrentCamera() {
        return currentCamera;
    }

    PushConstantData *getData() const {
        return data;
    }
};

