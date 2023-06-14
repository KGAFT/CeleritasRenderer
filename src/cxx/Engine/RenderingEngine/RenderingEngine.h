//
// Created by kgaft on 6/7/23.
//

#pragma once

#include <Vulkan/VulkanDevice/VulkanDevice.h>
#include <Vulkan/VulkanInstance.h>
#include <Vulkan/VulkanLogger/DefaultVulkanLoggerCallback.h>
#include "../../Window/Window.h"
namespace RenderEngine{
    struct EngineDevice{
        const char* name;
        VkPhysicalDevice base;
    };
    
    class Engine{
        private:
            static VulkanInstance* instance;
            static bool debugBuild;
            static Window* targetWindow;
        public:
            static void initializeContexts(const char* appName, Window* window, bool debugBuild);
            static void enumerateSupportedDevice(std::vector<EngineDevice>& output);
    };
}



