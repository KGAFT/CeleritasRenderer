//
// Created by Daniil on 23.04.2023.
//
#pragma once

#include <Vulkan/VulkanEndRenderPipeline.h>
#include <Util/ShaderLoader.h>
#include <Vulkan/VulkanImage/VulkanImage.h>

struct OutputConfig {
    unsigned int width;
    unsigned int height;
    unsigned int amount;
    unsigned int imagePerStepAmount;
};

class RenderPipeline {
private:
    int imagePerStepAmount;
    VulkanSyncManager *syncManager;
    VulkanEndRenderPipeline *endRenderPipeline;
    VulkanShader *shader;
    VulkanDevice *device;
    VulkanSwapChain *swapChain;
    std::vector<VulkanImage *> outputImages;
    bool isRender = false;
public:
    RenderPipeline(VulkanDevice *device, VulkanSwapChain *swapChain) : device(device), swapChain(swapChain) {
        syncManager = new VulkanSyncManager(device, swapChain);
    }

    void initialize(const char *pathToShadersDir, PipelineEndConfig &config, OutputConfig &outputConfig) {
        shader = ShaderLoader::loadShaders(pathToShadersDir, device);
        std::vector<VkImageView> renderTargets;

        for (unsigned int i = 0; i < outputConfig.amount; i++) {
            outputImages.push_back(VulkanImage::createImage(device, outputConfig.width, outputConfig.height));
            renderTargets.push_back(outputImages[outputImages.size() - 1]->getView());
        }
        if (renderTargets.size() == 0 and swapChain == nullptr) {
            throw std::runtime_error("Error: you need to specify output images if you haven't swapChain");
        }
        if (swapChain != nullptr) {
            endRenderPipeline = new VulkanEndRenderPipeline(device, syncManager, shader, &config, outputConfig.width,
                                                            outputConfig.height, swapChain->getSwapChainImageViews(), 1,
                                                            swapChain->getSwapChainImageFormat());
        } else {
            endRenderPipeline = new VulkanEndRenderPipeline(device, syncManager, shader, &config, outputConfig.width,
                                                            outputConfig.height, renderTargets,
                                                            outputConfig.imagePerStepAmount,
                                                            outputImages[0]->getFormat());
            this->imagePerStepAmount = outputConfig.imagePerStepAmount;
        }
    }

    VkCommandBuffer beginRender(bool isSamplersUpdate, bool isNewUniformBuffers){
        isRender = true;
        if (isSamplersUpdate)
        {
            endRenderPipeline->updateSamplers();
        }
        if (isNewUniformBuffers)
        {
            endRenderPipeline->updateUniforms();
        }
        VkCommandBuffer cmd = endRenderPipeline->beginRender();
        if (endRenderPipeline->getPushConstants().size() > 0)
        {
            endRenderPipeline->updatePcs();
        }
        if (endRenderPipeline->getUniformBuffers().size() > 0 or endRenderPipeline->getSamplers().size() > 0)
        {
            endRenderPipeline->bindImmediate();
        }
        return cmd;
    }
    void endRender(){
        endRenderPipeline->endRender();
        isRender = false;
    }

    void resize(int width, int height){
        vkDeviceWaitIdle(device->getDevice());
        if(swapChain==nullptr){
            int amount = outputImages.size();
            std::vector<VkImageView> renderTargets;
            for (const auto &item: outputImages){
                item->resize(width, height);
                renderTargets.push_back(item->getView());
            }
            endRenderPipeline->resized(width, height, renderTargets, this->imagePerStepAmount, outputImages[0]->getFormat());
        }
        else{
            endRenderPipeline->resized(width,height);
        }
    }
    std::vector<VkImageView>& getDepthOutput(){
        return endRenderPipeline->getDepthImages();
    }
    std::vector<VulkanImage *> &getOutputImages()  {
        return outputImages;
    }

    std::vector<VulkanSampler *> &getSamplers() {
        return endRenderPipeline->getSamplers();
    }

    std::vector<VulkanUniformBuffer *> &getUniforms() {
        return endRenderPipeline->getUniformBuffers();
    }

    std::vector<VulkanPushConstant *> &getPushConstants() {
        return endRenderPipeline->getPushConstants();
    }

    void updatePushConstants() {
        endRenderPipeline->updatePcs();
    }
    void updateSamplers(){
        endRenderPipeline->updateSamplers();
        if(isRender){
            endRenderPipeline->bindImmediate();
        }
    }
    void updateUniforms(){
        endRenderPipeline->updateUniforms();
        if(isRender){
            endRenderPipeline->bindImmediate();
        }
    }
};
