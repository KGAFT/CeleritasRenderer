//
// Created by kgaft on 6/6/23.
//

#pragma once

#include "RenderPipelineBuilder.h"
#include "Vulkan/VulkanImage/VulkanImage.h"
#include <Vulkan/VulkanEndRenderPipeline.h>
#include <Vulkan/VulkanDevice/VulkanDevice.h>
#include <Vulkan/VulkanSwapChain.h>
#include <Util/ShaderLoader.h>

namespace RenderEngine{
    class RenderPipelineSecond {
    public:
        RenderPipelineSecond(VulkanDevice* device, VulkanSwapChain swapChain);
    private:
        VulkanDevice* device;
        VulkanSwapChain* swapChain;
        VulkanEndRenderPipeline* endRenderPipeline;
        VulkanShader* shader;
        VulkanSyncManager* syncManager;
        std::vector<VulkanImage *> outputImages;
        std::vector<VkImageView> imageViews;
    public:
        void initialize(RenderEngine::RenderPipelineBuilder &builder);
    private:
        bool initializeRenderOutputs(RenderEngine::RenderPipelineBuilder &builder);
    };
}



