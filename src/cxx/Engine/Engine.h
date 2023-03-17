//
// Created by KGAFT on 3/17/2023.
//
#pragma once
#include "../Vulkan/VulkanInstance.h"
#include "../Window/Window.h"
#include "../Vulkan/VulkanDevice/VulkanDevice.h"
#include "../Vulkan/VulkanSwapChain.h"
#include "../Vulkan/VulkanLogger/DefaultVulkanLoggerCallback.h"
struct EngineDevice{
    std::string name;
    VkPhysicalDevice device;
    VkPhysicalDeviceProperties properties;
};

class Engine{
private:
    static inline VulkanInstance* instance = nullptr;
public:
    static void createInstance(const char* appName, bool debugBuild){
        instance = new VulkanInstance;
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

};