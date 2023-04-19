//
// Created by Daniil on 14.04.2023.
//
#pragma once

#include <Vulkan/VulkanDevice/VulkanDevice.h>
#include <Vulkan/VulkanGraphicsPipeline/VulkanShader/VulkanShader.h>
#include <Vulkan/VulkanSync/VulkanSyncManager.h>
#include <Util/ShaderLoader.h>
#include <Vulkan/VulkanImage/VulkanImage.h>
#include <Vulkan/VulkanEndRenderPipeline.h>
#include "glm/ext/matrix_clip_space.hpp"

#include <glm/ext/matrix_transform.hpp>


struct LightViewData{
    glm::mat4 lightSpaceMatrix;
    glm::mat4 worldMatrix;
};

class ShadowBufferPipeline {
private:
    VulkanDevice *device;
    VulkanShader *shader;
    VulkanSyncManager *syncManager;
    VulkanEndRenderPipeline* endRenderPipeline;
    LightViewData viewData{};
    VulkanImage* output;
public:
    ShadowBufferPipeline(VulkanDevice *device, int squareSideLen) : device(device) {
        shader = ShaderLoader::loadShaders("shaders/DirectLightShadowBuffer", device);
        syncManager = new VulkanSyncManager(device, nullptr);
        PipelineEndConfig config{};
        config.vertexInputs.push_back({0, 3, sizeof(float), VK_FORMAT_R32G32B32_SFLOAT});
        config.pushConstantInfos.push_back({VK_SHADER_STAGE_VERTEX_BIT, sizeof(LightViewData)});
        std::vector<VkImageView> renderTargets;
        output = VulkanImage::createImage(device, squareSideLen, squareSideLen);
        renderTargets.push_back(output->getView());
        endRenderPipeline = new VulkanEndRenderPipeline(device, syncManager, shader, &config, squareSideLen, squareSideLen, renderTargets,1, VK_FORMAT_R32G32B32_SFLOAT);
        endRenderPipeline->getPushConstants()[0]->setData(&viewData);
    }
    VkCommandBuffer beginRender(){
        VkCommandBuffer  cmd = endRenderPipeline->beginRender();
        endRenderPipeline->updatePcs();
        return cmd;
    }
    void endRender(){
        endRenderPipeline->endRender();
    }
    void recalculateMatrixForLightSource(glm::vec3 lightPos, int maxLightShadowDistance){
        glm::mat4 lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 0.01f, (float)maxLightShadowDistance);
        glm::mat4 lightView = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
        viewData.lightSpaceMatrix = lightProjection*lightView;
    }

    LightViewData &getViewData()  {
        return viewData;
    }

    void setViewData(const LightViewData &viewData) {
        ShadowBufferPipeline::viewData = viewData;
    }

};