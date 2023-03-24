//
// Created by KGAFT on 3/17/2023.
//
#pragma once

#include "../../../Vulkan/VulkanDevice/VulkanDevice.h"
#include "../../../Vulkan/VulkanSync/VulkanSyncManager.h"
#include "../../../Vulkan/VulkanEndRenderPipeline.h"
#include "../../../Vulkan/VulkanGraphicsPipeline/VulkanShader/VulkanShader.h"
#include "../../../Vulkan/VulkanImage/VulkanImage.h"
#include "../../../Util/ShaderLoader.h"
#include "../../../Vulkan/VulkanBuffers/VertexBuffer.h"

class UIPipeline{
private:
    VulkanDevice* device;
    VulkanSyncManager* syncManager;
    VulkanEndRenderPipeline* endRenderPipeline;
    VulkanShader* shader;
    VulkanImage* uiImage;

    float widthScale = 1;
    float heightScale = 1;
    int lastWidth = 0;
    int lastHeight = 0;
    VertexBuffer* testPrimitive;
public:
    UIPipeline(VulkanDevice *device, int startWidth, int startHeight) : device(device), lastWidth(startWidth), lastHeight(startHeight){
        shader = ShaderLoader::loadShaders("shaders/UIPipeline", device);
        syncManager = new VulkanSyncManager(device, nullptr);
        PipelineEndConfig endConfig{};
        endConfig.vertexInputs.push_back({0, 3, sizeof(float), VK_FORMAT_R32G32B32_SFLOAT});
        endConfig.vertexInputs.push_back({1, 4, sizeof(float), VK_FORMAT_R32G32B32A32_SFLOAT});
        uiImage = VulkanImage::createImage(device, startWidth, startHeight);
        uiImage = VulkanImage::createImage(device, startWidth, startHeight);
        std::vector<VkImageView> views;
        views.push_back(uiImage->getView());
        endRenderPipeline = new VulkanEndRenderPipeline(device, syncManager, shader, &endConfig, startWidth*widthScale, startHeight*heightScale, views, 1, uiImage->getFormat());
        
    }

    VkImageView update(){
        VkCommandBuffer cmd = endRenderPipeline->beginRender();
        testPrimitive->bind(cmd);
        testPrimitive->draw(cmd);
        endRenderPipeline->endRender();
        return uiImage->getView();
    }

    float getWidthScale()  {
        return widthScale;
    }

    void setWidthScale(float widthScale) {
        UIPipeline::widthScale = widthScale;
    }

    float getHeightScale()  {
        return heightScale;
    }

    void setHeightScale(float heightScale) {
        UIPipeline::heightScale = heightScale;
    }
    void resized(int width, int height){
        lastWidth = width;
        lastHeight = height;
    }
    void imageChanged(){
        delete uiImage;
    
        uiImage = VulkanImage::createImage(device, lastWidth*widthScale, lastHeight*heightScale);
        std::vector<VkImageView> views;
        views.push_back(uiImage->getView());
   
        endRenderPipeline->resized(lastWidth*widthScale, lastHeight*heightScale, &views, 1, uiImage->getFormat());
    }
};