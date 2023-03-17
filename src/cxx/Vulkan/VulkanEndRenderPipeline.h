//
// Created by KGAFT on 3/16/2023.
//
#pragma once
#include <array>
#include <vulkan/vulkan.h>
#include "VulkanSync/VulkanSyncManager.h"
#include "VulkanDevice/VulkanDevice.h"
#include "VulkanGraphicsPipeline/VulkanShader/VulkanShader.h"
#include "VulkanGraphicsPipeline/PipelineEndConfiguration.h"
#include "VulkanImmediateShaderData/UniformBuffers/VulkanUniformBuffer.h"
#include "VulkanImmediateShaderData/VulkanSampler/VulkanSampler.h"
#include "VulkanImmediateShaderData/PushConstants/VulkanPushConstant.h"
#include "VulkanRenderingPipeline/VulkanRenderPass.h"
#include "VulkanGraphicsPipeline/VulkanGraphicsPipeline.h"
#include "VulkanRenderingPipeline/VulkanRenderPipelineControl.h"
#include "VulkanDescriptors/VulkanDescriptors.h"
#include "VulkanImmediateShaderData/PushConstants/VulkanPushConstantManager.h"

class VulkanEndRenderPipeline : public IWindowResizeCallback{
private:
    VulkanDevice* device;
    VulkanSyncManager* syncManager;
    VulkanShader* shader;
    PipelineEndConfig* endConfig;
    std::vector<VkImageView>& imageViews;
    int currentWidth;
    int currentHeight;
    bool destroyed = false;
private:
    VulkanRenderPass* renderPass;
    VulkanGraphicsPipeline* graphicsPipeline;
    GraphicsPipelineConfigurer* configurer;
    VulkanRenderPipelineControl* control;

    std::vector<VulkanUniformBuffer*> uniformBuffers;
    std::vector<VulkanSampler*> samplers;
    std::vector<VulkanPushConstant*> pushConstants;

    int imagePerStepAmount = 0;
    VkFormat imageFormat;

    VkRenderPassBeginInfo renderPassInfo{};
    float clearColorValues[4] = {0,0,0,1};
    VkCommandBuffer currentCommandBuffer;
    VulkanDescriptors* descriptors = nullptr;
    VulkanPushConstantManager* manager;
public:
    VulkanEndRenderPipeline(VulkanDevice *device, VulkanSyncManager *syncManager, VulkanShader *shader,
                            PipelineEndConfig *endConfig, int startFrameBufferWidth, int startFrameBufferHeight, std::vector<VkImageView>& imageViews, int imagePerStepAmount, VkFormat imageFormat) : device(device), imageViews(imageViews), imageFormat(imageFormat), syncManager(syncManager), shader(shader),
                                                            endConfig(endConfig), currentWidth(startFrameBufferWidth), currentHeight(startFrameBufferHeight) {
        createRenderPass(startFrameBufferWidth, startFrameBufferHeight, imagePerStepAmount, imageFormat);
        createGraphicsPipeline(endConfig, startFrameBufferWidth, startFrameBufferHeight);
        createControl();
        if(configurer->getDescriptorSetLayout()!=VK_NULL_HANDLE){
            descriptors = new VulkanDescriptors(device, configurer->getDescriptorSetLayout(), syncManager->getCurrentMode());
        }

        manager = new VulkanPushConstantManager();
        createPushConstants(endConfig);

        for (const auto &item: pushConstants){
            manager->registerPushConstant(item);
        }

        createSamplers(endConfig);
        createUniforms(endConfig);
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    }
    /**
     * Update immediate  data before this
     */
    VkCommandBuffer beginRender(){
        std::pair rendInfo = control->beginRender();
        renderPassInfo.renderPass = renderPass->getRenderPass();
        renderPassInfo.framebuffer = rendInfo.second;
        renderPassInfo.renderArea.offset = {0, 0};
        renderPassInfo.renderArea.extent = {static_cast<uint32_t>(currentWidth), static_cast<uint32_t>(currentHeight)};
        std::array<VkClearValue, 2> clearValues{};
        clearValues[0].color = {clearColorValues[0], clearColorValues[1], clearColorValues[2], clearColorValues[3]};
        clearValues[1].depthStencil = {1.0f, 0};
        renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
        renderPassInfo.pClearValues = clearValues.data();

        vkCmdBeginRenderPass(rendInfo.first, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
        vkCmdBindPipeline(rendInfo.first, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline->getGraphicsPipeline());
        manager->loadConstantsToShader(rendInfo.first, configurer->getPipelineLayout());
        if(descriptors!= nullptr){
            descriptors->writeDescriptorObjects(reinterpret_cast<IDescriptorObject **>(uniformBuffers.data()), uniformBuffers.size(), control->getCurrentCmd());
            descriptors->writeDescriptorObjects(reinterpret_cast<IDescriptorObject **>(samplers.data()), samplers.size(), control->getCurrentCmd());
            descriptors->bind(control->getCurrentCmd(), rendInfo.first, configurer->getPipelineLayout());
        }

        currentCommandBuffer = rendInfo.first;
        return rendInfo.first;
    }
    void endRender(){
        vkCmdEndRenderPass(currentCommandBuffer);

        control->endRender();

    }
    void resized(int width, int height) override {
        vkDeviceWaitIdle(device->getDevice());
        currentWidth = width;
        currentHeight = height;
        if(syncManager->getSwapChain()!=nullptr){
            syncManager->getSwapChain()->recreate(width, height);
            imageViews = syncManager->getSwapChain()->getSwapChainImageViews();
            imagePerStepAmount = 1;
            imageFormat = syncManager->getSwapChain()->getSwapChainImageFormat();
        }
        renderPass->recreate(imageViews, width, height, imagePerStepAmount, &imageFormat, 1);
        graphicsPipeline->recreate(PipelineConfiguration::defaultPipelineConfigInfo(width, height), renderPass);
        control->setRenderPass(renderPass);

    }
    VkImageView getCurrentImage(){
        return imageViews[control->getCurrentCmd()];
    }
    void destroy(){
        if(!destroyed){
            vkDeviceWaitIdle(device->getDevice());
            for (const auto &item: uniformBuffers){
                delete item;
            }
            uniformBuffers.clear();
            for (const auto &item: samplers){
                delete item;
            }
            samplers.clear();
            for (const auto &item: pushConstants){
                delete item;
            }
            pushConstants.clear();
            if(manager!= nullptr){
                delete manager;
            }
            if(descriptors!=nullptr){
                delete descriptors;
            }
            delete control;
            delete graphicsPipeline;
            delete renderPass;
            delete configurer;
            destroyed = true;
        }
    }
    ~VulkanEndRenderPipeline(){
        destroy();
    }

    std::vector<VulkanSampler *> &getSamplers() {
        return samplers;
    }

    std::vector<VulkanUniformBuffer *> &getUniformBuffers() {
        return uniformBuffers;
    }

    std::vector<VulkanPushConstant *> &getPushConstants() {
        return pushConstants;
    }

private:

    void createPushConstants(PipelineEndConfig* endConfig){
        for (const auto &item: endConfig->pushConstantInfos){
            pushConstants.push_back(new VulkanPushConstant(item.size, item.shaderStages));
        }
    }
    void createUniforms(PipelineEndConfig* endConfig){
        for (const auto &item: endConfig->uniformBuffers){
            uniformBuffers.push_back(new VulkanUniformBuffer(device, item.size, item.shaderStages, item.binding, syncManager->getCurrentMode()));
        }
    }

    void createSamplers(PipelineEndConfig* endConfig){
        for (const auto &item: endConfig->samplers){
            samplers.push_back(new VulkanSampler(device, item.binding));
        }
    }

    void createRenderPass(int width, int height, int imagePerStepAmount, VkFormat imageFormat){
        this->imagePerStepAmount = imagePerStepAmount;
        renderPass = new VulkanRenderPass(device, imageViews, width, height, imagePerStepAmount, &imageFormat, 1);
    }
    void createGraphicsPipeline(PipelineEndConfig *endConfig, int width, int height){
        configurer = new GraphicsPipelineConfigurer(device, endConfig);
        graphicsPipeline = new VulkanGraphicsPipeline(device, configurer, shader, PipelineConfiguration::defaultPipelineConfigInfo(width, height), renderPass);
    }
    void createControl(){
        control = new VulkanRenderPipelineControl(syncManager, device, renderPass);
    }
};