//
// Created by KGAFT on 3/17/2023.
//
#pragma once

#include <Vulkan/VulkanInstance.h>
#include "../Window/Window.h"
#include <Vulkan/VulkanDevice/VulkanDevice.h>
#include <Vulkan/VulkanSwapChain.h>
#include <Vulkan/VulkanLogger/DefaultVulkanLoggerCallback.h>
#include <Vulkan/VulkanImage/VulkanImage.h>
#include "../Util/ModelLoader.h"
#include "Pipelines/AssemblyPipeline.h"
#include "Pipelines/GBufferPipeline.h"

#include "Camera/CameraManager.h"
#include "ShadowManager.h"
#include "../TestKeyboardCallback.h"
#include "Pipelines/GameAssemblyPipeline.h"

struct EngineDevice
{
    std::string name;
    VkPhysicalDevice device;
    VkPhysicalDeviceProperties properties;
};

class Engine : public IWindowResizeCallback
{
private:
    static inline VulkanInstance *instance = nullptr;
    static inline bool debugBuild = false;

public:
    static void createInstance(const char *appName, bool debugBuild)
    {
        instance = new VulkanInstance;
        Engine::debugBuild = debugBuild;
        uint32_t extCount;
        const char **exts = glfwGetRequiredInstanceExtensions(&extCount);
        std::vector<const char *> extensions;
        for (int i = 0; i < extCount; ++i)
        {
            extensions.push_back(exts[i]);
        }

        if (!instance->createInstance(appName, debugBuild, extensions))
        {
            throw std::runtime_error("Failed to init Vulkan API");
        }
        if (debugBuild)
        {
            VulkanLogger::registerCallback(new DefaultVulkanLoggerCallback);
        }
    }

    static std::vector<EngineDevice> enumSupportedDevices(Window *window)
    {
        if (instance == nullptr)
        {
            throw std::runtime_error("Error: you need to create instance firstly");
        }
        std::vector<EngineDevice> result;
        for (const auto &item : VulkanDevice::enumerateSupportedDevices(instance->getInstance(), window->getWindowSurface(instance->getInstance())))
        {
            result.push_back({item.second.deviceName, item.first, item.second});
        }
        return result;
    }

private:
    VulkanDevice *device;
    Window *window;
    VulkanSwapChain *swapChain;
    VulkanImage *skyPlaceHolder;
    VulkanImage *gamePlaceHolder;
    AssemblyPipeline *asmPipeline;
    GBufferPipeline *gbPipeline;
    GameAssemblyPipeline *gbaPipeline;
    ShadowManager* shadowManager;
    CameraManager *manager;
    std::vector<Mesh *> meshes;
    Material material;
    long long frameCounter = 0;
public:
    Engine(EngineDevice &deviceToCreate, Window *window) : window(window)
    {
        if (instance == nullptr)
        {
            throw std::runtime_error("Error: you need to create instance firstly");
        }
        device = new VulkanDevice(deviceToCreate.device, window->getWindowSurface(instance->getInstance()), instance->getInstance(), debugBuild);
        swapChain = new VulkanSwapChain(device, window->getWidth(), window->getHeight());

        ModelLoader loader(device);

        meshes = loader.loadModel("models/pokedex/pokedex.gltf", false);
        material.setAlbedoTexture(VulkanImage::loadTexture("models/pokedex/textures/basecolor.tga", device));
        material.setMetallicTexture(VulkanImage::loadTexture("models/pokedex/textures/metallic.tga", device));
        material.setRoughnessTexture(VulkanImage::loadTexture("models/pokedex/textures/roughness.tga", device));
        material.setNormalMap(VulkanImage::loadTexture("models/pokedex/textures/normal.tga", device));
        material.setEmissiveTexture(VulkanImage::loadTexture("models/pokedex/textures/emissive.tga", device));
        //material.setAoTexture(VulkanImage::loadTexture("models/pokedex/textures/ao.tga", device));

        meshes[0]->setMaterial(&material);

        // meshes = loader.loadModel("models/Helmet/DamagedHelmet.gltf", true);
        // VulkanImage* albedoIm = VulkanImage::loadTexture("models/Helmet/Default_albedo.jpg", device);
        // for (const auto &item: meshes){
        //    item->getMaterial()->setAoTexture(nullptr);
        //    item->getMaterial()->setOpacityMapTexture(nullptr);
        //    item->getMaterial()->setAlbedoTexture(albedoIm);
        //}
        TestKeyboardCallback *keyBoardCB = new TestKeyboardCallback(Window::getInstance());
        Window::getInstance()->registerKeyCallback(keyBoardCB);
        skyPlaceHolder = VulkanImage::loadTexture("models/bluecloud_ft.jpg", device);
        gamePlaceHolder = VulkanImage::loadTexture("shaders/game.png", device);
        asmPipeline = new AssemblyPipeline(device, swapChain, Window::getInstance()->getWidth(), Window::getInstance()->getHeight());

        shadowManager = new ShadowManager(device, 4096, Window::getInstance()->getWidth(), Window::getInstance()->getWidth());

        gbPipeline = new GBufferPipeline(device, Window::getInstance()->getWidth(), Window::getInstance()->getHeight());
        manager = new CameraManager(&gbPipeline->getPcData());
        gbaPipeline = new GameAssemblyPipeline(device, Window::getInstance()->getWidth(), Window::getInstance()->getHeight());
        gbaPipeline->setGBufferPipeline(gbPipeline);
        gbaPipeline->setAo(shadowManager->getOutput());
        shadowManager->setupLightView(glm::vec3(0.0f,8.0f,0.9f), 75);
        asmPipeline->setGamePlaceHolder(gbaPipeline->getOutputImages()[0]);
        asmPipeline->setUiPlaceHolder(skyPlaceHolder);
        asmPipeline->getData().mode = 1;
        asmPipeline->updateSamplers();
        gbaPipeline->getLightConfig().enabledPoints = 1;
        gbaPipeline->getLightConfig().enabledPoints = 1;
        gbaPipeline->getLightConfig().pointLights[0].color = glm::vec3(1, 1, 1);
        gbaPipeline->getLightConfig().pointLights[0].position = glm::vec3(0, 5, -5);
        gbaPipeline->getLightConfig().pointLights[0].intensity = 1000;
        gbaPipeline->getLightConfig().emissiveIntensity = 6;

        meshes[0]->setPosition(glm::vec3(0.0f, 1.5f, 0.0));
        window->registerResizeCallback(this);
    }

    void update()
    {

        manager->update();
        gbaPipeline->getVertexConfig().cameraPosition = gbPipeline->getPcData().cameraPosition;
        gbPipeline->beginRender();
        gbPipeline->setSkyBox(skyPlaceHolder);
        gbPipeline->processMesh(meshes[0]);
        gbPipeline->endRender();
        shadowManager->processMesh(meshes[0], gbPipeline->getPcData().viewMatrix, gbPipeline->getPcData().cameraPosition);

        gbaPipeline->update();

        asmPipeline->getData().mode = 1;
        asmPipeline->update();
    }

    void resized(int width, int height) override
    {
        vkDeviceWaitIdle(device->getDevice());
        gbPipeline->resize(width, height);
        gbaPipeline->resize(width, height);
        gbaPipeline->setGBufferPipeline(gbPipeline);
        shadowManager->resizeOutput(width, height);
        gbaPipeline->setAo(shadowManager->getOutput());
        asmPipeline->resize(width, height);
        asmPipeline->setGamePlaceHolder(gbaPipeline->getOutputImages()[0]);
        frameCounter++;
    }
};
