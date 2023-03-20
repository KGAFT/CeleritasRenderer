//
// Created by KGAFT on 3/17/2023.
//
#pragma once
#include "../Vulkan/VulkanInstance.h"
#include "../Window/Window.h"
#include "../Vulkan/VulkanDevice/VulkanDevice.h"
#include "../Vulkan/VulkanSwapChain.h"
#include "../Vulkan/VulkanLogger/DefaultVulkanLoggerCallback.h"
#include "Pipelines/AssemblyPipeline/AssemblyPipeline.h"
#include "../Vulkan/VulkanImage/VulkanImage.h"
#include "Pipelines/UIPipeline/UIPipeline.h"

struct EngineDevice{
    std::string name;
    VkPhysicalDevice device;
    VkPhysicalDeviceProperties properties;
};

class Engine{
private:
    static inline VulkanInstance* instance = nullptr;
    static inline bool debugBuild = false;
public:
    static void createInstance(const char* appName, bool debugBuild){
        instance = new VulkanInstance;
        Engine::debugBuild = debugBuild;
        if(!instance->createInstance(appName, debugBuild)){
            throw std::runtime_error("Failed to init Vulkan API");
        }
        if(debugBuild){
            VulkanLogger::registerCallback(new DefaultVulkanLoggerCallback);
        }

    }
    static std::vector<EngineDevice> enumSupportedDevices(Window* window){
        if(instance== nullptr){
            throw std::runtime_error("Error: you need to create instance firstly");
        }
        std::vector<EngineDevice> result;
        for (const auto &item: VulkanDevice::enumerateSupportedDevices(instance->getInstance(), window)){
            result.push_back({item.second.deviceName, item.first, item.second});
        }
        return result;
    }
private:
    VulkanDevice* device;
    Window* window;
    VulkanSwapChain* swapChain;
    AssemblyPipeline* assemblyPipeline;
    VulkanImage* uiPlaceHolder;
    VulkanImage* gamePlaceHolder;
    UIPipeline* uiPipeline;
    long long frameCounter = 0;
public:
    Engine(EngineDevice& deviceToCreate, Window* window) : window(window){
        if(instance== nullptr){
            throw std::runtime_error("Error: you need to create instance firstly");
        }
        device = new VulkanDevice(deviceToCreate.device, window, instance->getInstance(), debugBuild);
        swapChain = new VulkanSwapChain(device, window->getWidth(), window->getHeight());
        uiPlaceHolder = VulkanImage::loadTexture("shaders/ui.png", device);
        gamePlaceHolder = VulkanImage::loadTexture("shaders/ui.png", device);
        assemblyPipeline = new AssemblyPipeline(device, swapChain, window);
        assemblyPipeline->getGameSampler()->setSamplerImageView(gamePlaceHolder->getView());
        assemblyPipeline->getUISampler()->setSamplerImageView(uiPlaceHolder->getView());
        assemblyPipeline->getCorrect().outCorrectAmplifier.a = 0.5f;
        uiPipeline = new UIPipeline(device, Window::getInstance()->getWidth(), Window::getInstance()->getHeight());

    }

    void update(){
        VkImageView imageView;
        if(frameCounter%2==0){
            imageView = uiPipeline->update().first;
        }
        else{
            imageView = uiPipeline->update().second;
        }
        assemblyPipeline->getUISampler()->setSamplerImageView(imageView);
        assemblyPipeline->update();
        frameCounter++;
    }
};