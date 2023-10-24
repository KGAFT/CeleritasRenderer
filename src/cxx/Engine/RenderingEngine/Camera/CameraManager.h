//
// Created by Daniil on 26.12.2022.
//

#pragma once

#include "Camera.h"
#include "CameraMovementCallBack.h"
#include "CameraRotationCallBack.h"
#include "Window/Window.hpp"
#include "CameraResizeCallBack.h"
#include "../RenderingPipelines/GBufferPipeline.h"

namespace RenderEngine {
    class CameraManager {
    private:
        static inline CameraManager *cameraManagerInstance = nullptr;

    private:
        float fov = 90.0f;
        Camera *currentCamera;
        CameraMovementCallBack *currentCameraMovementCallback;
        CameraRotationCallBack *currentCameraRotationCallBack;
        CameraResizeCallBack *cameraResizeCallBack;
        WorldTransformData *data;

    public:
        CameraManager(WorldTransformData *data, Window *window) {
            currentCamera = new Camera(glm::vec3(0, 0, 2.0f), window->getWidth(),
                                       window->getHeight());
            currentCameraMovementCallback = new CameraMovementCallBack(currentCamera, 0.02f);
            currentCameraRotationCallBack = new CameraRotationCallBack(currentCamera, 0.5f);
            cameraResizeCallBack = new CameraResizeCallBack(currentCamera);
            this->data = data;
            window->addResizeCallback(cameraResizeCallBack);
            window->getInputSystem().registerKeyCallback(currentCameraMovementCallback);
            window->getInputSystem().registerMouseCallback(currentCameraRotationCallBack);
        }

        void update() {
            glm::mat4 camMat = currentCamera->getCameraMatrix(fov, 0.001f, 100.0f);
            data->viewMatrix = camMat;
            data->worldMatrix = glm::mat4(1.0f);
            data->cameraPosition = currentCamera->getPosition();
        }

        void setCurrentCamera(Camera *currentCamera) {
            this->currentCamera = currentCamera;
            currentCameraRotationCallBack->setCamera(this->currentCamera);
            currentCameraMovementCallback->setCamera(this->currentCamera);
            cameraResizeCallBack->setCurrentCamera(this->currentCamera);
        }

        Camera *getCurrentCamera() {
            return currentCamera;
        }

        WorldTransformData *getData() {
            return data;
        }
    };


}
