//
// Created by KGAFT on 3/17/2023.
//
#pragma once

#include "../Vulkan/VulkanInstance.h"
#include "../Window/Window.h"
#include "../Vulkan/VulkanDevice/VulkanDevice.h"
#include "../Vulkan/VulkanSwapChain.h"
#include "../Vulkan/VulkanLogger/DefaultVulkanLoggerCallback.h"
#include "../Vulkan/VulkanImage/VulkanImage.h"
#include "../Util/ModelLoader.h"
#include "Pipelines/AssemblyPipeline.h"
#include "Camera/CameraManager.h"

#include "../TestKeyboardCallback.h"
#include "Pipelines/GBufferPipeline.h"
#include "Pipelines/GameAssemblyPipeline.h"

struct EngineDevice {
    std::string name;
    VkPhysicalDevice device;
    VkPhysicalDeviceProperties properties;
};

class Engine : public IWindowResizeCallback {
private:
    static inline VulkanInstance *instance = nullptr;
    static inline bool debugBuild = false;

public:
    static void createInstance(const char *appName, bool debugBuild) {
        instance = new VulkanInstance;
        Engine::debugBuild = debugBuild;
        if (!instance->createInstance(appName, debugBuild)) {
            throw std::runtime_error("Failed to init Vulkan API");
        }
        if (debugBuild) {
            VulkanLogger::registerCallback(new DefaultVulkanLoggerCallback);
        }
    }

    static std::vector<EngineDevice> enumSupportedDevices(Window *window) {
        if (instance == nullptr) {
            throw std::runtime_error("Error: you need to create instance firstly");
        }
        std::vector<EngineDevice> result;
        for (const auto &item: VulkanDevice::enumerateSupportedDevices(instance->getInstance(), window)) {
            result.push_back({item.second.deviceName, item.first, item.second});
        }
        return result;
    }

private:
    VulkanDevice *device;
    Window *window;
    VulkanSwapChain *swapChain;
    VulkanImage *uiPlaceHolder;
    VulkanImage *gamePlaceHolder;
    AssemblyPipeline *asmPipeline;
    GBufferPipeline *gbPipeline;
    GameAssemblyPipeline* gbaPipeline;

    CameraManager manager;
    std::vector<Mesh *> meshes;
    PushConstantData pcData;
    Material material;
    long long frameCounter = 0;
public:
    Engine(EngineDevice &deviceToCreate, Window *window) : window(window), manager(&pcData) {
        pcData.worldMatrix = glm::mat4(1.0f);
        if (instance == nullptr) {
            throw std::runtime_error("Error: you need to create instance firstly");
        }
        device = new VulkanDevice(deviceToCreate.device, window, instance->getInstance(), debugBuild);
        swapChain = new VulkanSwapChain(device, window->getWidth(), window->getHeight());

        ModelLoader loader(device);
        meshes = loader.loadModel("models/pokedex/pokedex.gltf", false);

        material.setAlbedoTexture(VulkanImage::loadTexture("models/pokedex/textures/basecolor.tga", device));
        material.setMetallicTexture(VulkanImage::loadTexture("models/pokedex/textures/metallic.tga", device));
        material.setRoughnessTexture(VulkanImage::loadTexture("models/pokedex/textures/roughness.tga", device));
        material.setNormalMap(VulkanImage::loadTexture("models/pokedex/textures/normal.tga", device));
       // material.setAoTexture(VulkanImage::loadTexture("models/pokedex/textures/ao.tga", device));
        material.setEmissiveTexture(VulkanImage::loadTexture("models/pokedex/textures/emissive.tga", device));
        meshes[0]->setMaterial(&material);

        TestKeyboardCallback *keyBoardCB = new TestKeyboardCallback(Window::getInstance());
        Window::getInstance()->registerKeyCallback(keyBoardCB);
        uiPlaceHolder = VulkanImage::loadTexture("shaders/ui.png", device);
        gamePlaceHolder = VulkanImage::loadTexture("shaders/game.png", device);
        asmPipeline = new AssemblyPipeline(device, swapChain, Window::getInstance());
        asmPipeline->SetGamePlaceHolder(gamePlaceHolder);
        asmPipeline->SetUiPlaceHolder(uiPlaceHolder);
        asmPipeline->getData().mode = 1;
        asmPipeline->prepare();


        gbPipeline = new GBufferPipeline(device, Window::getInstance()->getWidth(), Window::getInstance()->getHeight());
        gbPipeline->populateSamplers(&material);
        gbPipeline->updateSamplers();

        gbaPipeline = new GameAssemblyPipeline(device, Window::getInstance()->getWidth(), Window::getInstance()->getHeight());
        gbaPipeline->setGBufferPipeline(gbPipeline);
        gbaPipeline->getConfig().enabledPoints = 1;
        gbaPipeline->getConfig().pointLights[0].color = glm::vec3(1,1,1);
        gbaPipeline->getConfig().pointLights[0].position = glm::vec3(0, 5, -5);
        gbaPipeline->getConfig().pointLights[0].intensity = 1000;
        gbaPipeline->getConfig().emissiveIntensity = 6;


        asmPipeline->SetGamePlaceHolder(gbaPipeline->getOutput());
        asmPipeline->updateSamplers();


        window->registerResizeCallback(this);
    }

    void update() {

        manager.update();
        manager.getData()->worldMatrix = glm::scale(manager.getData()->worldMatrix, glm::vec3(0.25f, 0.25f, 0.25f));
        manager.getData()->worldMatrix = glm::translate(manager.getData()->worldMatrix, glm::vec3(-1,0,0));
        gbPipeline->setWorldViewData(manager.getData());

        VkCommandBuffer cmd = gbPipeline->beginRender();
        meshes[0]->draw(cmd);
        gbPipeline->endRender();
        gbaPipeline->getVertexConfig()->cameraPosition = manager.getData()->cameraPosition;
        gbaPipeline->update();

        asmPipeline->update();
        frameCounter++;
    }

    void resized(int width, int height) override {
        vkDeviceWaitIdle(device->getDevice());
        asmPipeline->resize(width,height);
        gbPipeline->resize(width,height);
        asmPipeline->SetGamePlaceHolder(gbPipeline->getNormalMapImage());
        asmPipeline->updateSamplers();
    }
};