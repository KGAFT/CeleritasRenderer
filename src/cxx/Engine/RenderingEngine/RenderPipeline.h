

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
    class RenderPipeline {
    public:
        RenderPipeline(VulkanDevice* device, VulkanSwapChain* swapChain);
    private:
        VulkanDevice* device;
        VulkanSwapChain* swapChain;
        VulkanEndRenderPipeline* endRenderPipeline;
        VulkanShader* shader;
        VulkanSyncManager* syncManager;
        std::vector<VulkanImage *> outputImages;
        std::vector<VkImageView> imageViews;
        
        unsigned int imagePerStepAmount;
    public:
         void resize(int width, int height);
    protected:
        void initialize(RenderEngine::RenderPipelineBuilder& builder);
        VulkanDescriptorSet* acquireDescriptorSet();
        void releaseDescriptorSet(VulkanDescriptorSet* descriptorSet);
        std::vector<VulkanPushConstant*> &getPushConstants();
        VkCommandBuffer beginRender();
        void endRenderPass();
        void endRender();
        std::vector<VkImage>& getDepthImages();
        std::vector<VulkanImage *> &getOutputImages();
        void initUi(GLFWwindow* window);
        void updatePushConstants();
        void bindDescriptorSet(VulkanDescriptorSet* descriptorSet);
        VkPipelineLayout getPipelineLayout();
    private:
        bool initializeRenderOutputs(RenderEngine::RenderPipelineBuilder &builder);
    };
}