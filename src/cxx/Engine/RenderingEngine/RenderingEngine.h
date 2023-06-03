//
// Created by kgaft on 6/3/23.
//

#pragma once

#include <Vulkan/VulkanInstance.h>
#include "RenderPipelineImplementation/AssemblyPipeline.h"
#include "ShadowManager.h"
#include "RenderPipelineImplementation/GBufferPipeline.h"
#include "RenderPipelineImplementation/GameAssebmlyPipeline.h"
#include <Vulkan/VulkanDevice/VulkanDevice.h>
#include <Vulkan/VulkanSwapChain.h>
#include <Vulkan/VulkanLogger/DefaultVulkanLoggerCallback.h>
#include <Vulkan/VulkanImage/VulkanImage.h>

namespace RenderingEngine{
    struct EngineDevice {
        std::string name;
        VkPhysicalDevice device;
        VkPhysicalDeviceProperties properties;
    };

    class Engine{
    private:
        static inline VulkanInstance *instance = nullptr;
        static inline bool debugBuild = false;
    public:
        static void createInstance(const char *appName, bool debugBuild) {
            instance = new VulkanInstance;
            Engine::debugBuild = debugBuild;
            uint32_t extCount;
            const char **exts = glfwGetRequiredInstanceExtensions(&extCount);
            std::vector<const char *> extensions;
            for (int i = 0; i < extCount; ++i) {
                extensions.push_back(exts[i]);
            }


            if (!instance->createInstance(appName, debugBuild, extensions)) {
                throw std::runtime_error("Failed to init Vulkan API");
            }
            if (debugBuild) {
                VulkanLogger::registerCallback(new DefaultVulkanLoggerCallback);
            }
        }

        static void enumSupportedDevices(Window *window, std::vector<EngineDevice>& result) {
            if (instance == nullptr) {
                throw std::runtime_error("Error: you need to create instance firstly");
            }
            for (const auto &item: VulkanDevice::enumerateSupportedDevices(instance->getInstance(),
                                                                           window->getWindowSurface(
                                                                                   instance->getInstance()))) {
                result.push_back({item.second.deviceName, item.first, item.second});
            }
        }
    private:
        AssemblyPipeline *asmPipeline;
        GBufferPipeline *gBufferPipeline;
        GameAssemblyPipeline* gameAssemblyPipeline;
        CameraManager *manager;
        ShadowManager* shadowManager;
        VulkanDevice *device;

        Window *window;
        VulkanSwapChain *swapChain;
        Engine(EngineDevice &deviceToCreate, Window *window) : window(window) {
            if (instance == nullptr) {
                throw std::runtime_error("Error: you need to create instance firstly");
            }
            device = new VulkanDevice(deviceToCreate.device, window->getWindowSurface(instance->getInstance()),
                                      instance->getInstance(), debugBuild);
            swapChain = new VulkanSwapChain(device, window->getWidth(), window->getHeight());
            gBufferPipeline = new GBufferPipeline(device, window->getWidth(), window->getHeight());
            gameAssemblyPipeline = new GameAssemblyPipeline(device, window->getWidth(), window->getHeight());
            shadowManager = new ShadowManager(device, 4096, window->getWidth(), window->getHeight());

        }
    };
};