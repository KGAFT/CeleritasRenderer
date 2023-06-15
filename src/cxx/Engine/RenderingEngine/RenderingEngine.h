//
// Created by kgaft on 6/7/23.
//

#pragma once

#include <Vulkan/VulkanDevice/VulkanDevice.h>
#include <Vulkan/VulkanInstance.h>
#include <Vulkan/VulkanLogger/DefaultVulkanLoggerCallback.h>
#include <Vulkan/VulkanSwapChain.h>
#include "../../Window/Window.h"
#include "RenderingPipelines/AssemblyPipeline.h"
#include "RenderingPipelines/GBufferPipeline.h"
#include "RenderingPipelines/GameAssemblyPipeline.h"
#include "RenderingPipelines/SkyboxPipeline.h"
#include "RenderingPipelines/DirectLightShadows/ShadowManager.h"
namespace RenderEngine{
    struct EngineDevice{
        const char* name;
        VkPhysicalDevice base;
    };
    
    class Engine{
        public:
            static void initializeContexts(const char* appName, Window* window, bool debugBuild);
            static void enumerateSupportedDevice(std::vector<EngineDevice>& output);
        private:
            static VulkanInstance* instance;
            static bool debugBuild;
            static Window* targetWindow;
        public:
            Engine(EngineDevice& targetDevice);
        private:
            VulkanDevice* device;
            VulkanSwapChain* swapChain;
            AssemblyPipeline* assemblyPipeline;
            GBufferPipeline* gBufferPipeline;
            GameAssemblyPipeline* gameAssemblyPipeline;
            ShadowManager* shadowManager;
            SkyboxPipeline* skyboxPipeline;
        private:
            void setupPipelinesConnections();
    };
}



