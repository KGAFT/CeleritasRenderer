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

struct EngineDevice
{
    std::string name;
    VkPhysicalDevice device;
    VkPhysicalDeviceProperties properties;
};

class Engine
{
private:
    static inline VulkanInstance *instance = nullptr;
    static inline bool debugBuild = false;

public:
    static void createInstance(const char *appName, bool debugBuild)
    {
        instance = new VulkanInstance;
        Engine::debugBuild = debugBuild;
        if (!instance->createInstance(appName, debugBuild))
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
        for (const auto &item : VulkanDevice::enumerateSupportedDevices(instance->getInstance(), window))
        {
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
    CameraManager manager;
    std::vector<Mesh *> meshes;
    PushConstantData pcData;
    Material material;
    long long frameCounter = 0;
public:
    Engine(EngineDevice &deviceToCreate, Window *window) : window(window), manager(&pcData)
    {
        if (instance == nullptr)
        {
            throw std::runtime_error("Error: you need to create instance firstly");
        }
        device = new VulkanDevice(deviceToCreate.device, window, instance->getInstance(), debugBuild);
        swapChain = new VulkanSwapChain(device, window->getWidth(), window->getHeight());
        // uiPlaceHolder = AssetLoader::loadTextures(device, "C:/Users/Daniil/Desktop/model.sc");

        // ModelLoader loader(device);
        // meshes = loader.loadModel("C:/Users/Daniil/Desktop/vaze/Vaze.fbx");

        // material.setAlbedoTexture(VulkanImage::loadTexture("C:/Users/Daniil/Desktop/vaze/albedo.tga", device));
        // material.setMetallicTexture(VulkanImage::loadTexture("C:/Users/Daniil/Desktop/vaze/metallic.tga", device));
        //  material.setRoughnessTexture(VulkanImage::loadTexture("C:/Users/Daniil/Desktop/vaze/roughness.tga", device));
        //  material.setNormalMap(VulkanImage::loadTexture("C:/Users/Daniil/Desktop/vaze/normal.tga", device));
        // meshes[0]->setMaterial(&material);
        TestKeyboardCallback *keyBoardCB = new TestKeyboardCallback(Window::getInstance());
        Window::getInstance()->registerKeyCallback(keyBoardCB);
        uiPlaceHolder = VulkanImage::loadTexture("shaders/ui.png", device);
        gamePlaceHolder = VulkanImage::loadTexture("shaders/game.png", device);
        asmPipeline = new AssemblyPipeline(device, swapChain, Window::getInstance());
        asmPipeline->SetGamePlaceHolder(gamePlaceHolder);
        asmPipeline->SetUiPlaceHolder(uiPlaceHolder);
        asmPipeline->prepare();

    }

    void update()
    {

        manager.update();
        if(frameCounter%2==0){
            asmPipeline->SetGamePlaceHolder(uiPlaceHolder);
            asmPipeline->SetUiPlaceHolder(gamePlaceHolder);
        }
        else{
            asmPipeline->SetGamePlaceHolder(gamePlaceHolder);
            asmPipeline->SetUiPlaceHolder(uiPlaceHolder);
        }
        asmPipeline->updateSamplers();
        asmPipeline->update();
        frameCounter++;
    }
};