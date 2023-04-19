//
// Created by Daniil on 15.04.2023.
//
#pragma once

#include <glm/glm.hpp>
#include <Vulkan/VulkanDevice/VulkanDevice.h>
#include <Vulkan/VulkanGraphicsPipeline/VulkanShader/VulkanShader.h>
#include <Vulkan/VulkanSync/VulkanSyncManager.h>
#include <Util/ShaderLoader.h>
#include <Vulkan/VulkanImage/VulkanImage.h>
#include <Vulkan/VulkanEndRenderPipeline.h>

struct LightTransformData {
    glm::mat4 lightSpaceMatrix;
    glm::mat4 worldMatrix;
    glm::vec3 cameraPosition;
};

struct ShadowAssemblerConfig {
    alignas(16) glm::vec3 lightPosition;
};

class ShadowToAOAssemblePipeline {
private:
    VulkanDevice *device;
    VulkanShader *shader;
    VulkanSyncManager *syncManager;
    VulkanEndRenderPipeline *endRenderPipeline;
    VulkanImage *output;
    LightTransformData lightView{};
    ShadowAssemblerConfig shadowConfig{};
public:
    ShadowToAOAssemblePipeline(VulkanDevice *device, int width, int height) : device(device) {
        shader = ShaderLoader::loadShaders("shaders/ShadowAssemblePipeline", device);
        syncManager = new VulkanSyncManager(device, nullptr);
        output = VulkanImage::createImage(device, width, height);
        PipelineEndConfig endConfig{};
        endConfig.vertexInputs.push_back({0, 3, sizeof(float), VK_FORMAT_R32G32B32_SFLOAT});
        endConfig.vertexInputs.push_back({1, 2, sizeof(float), VK_FORMAT_R32G32_SFLOAT});
        endConfig.vertexInputs.push_back({2, 3, sizeof(float), VK_FORMAT_R32G32B32_SFLOAT});
        endConfig.pushConstantInfos.push_back({VK_SHADER_STAGE_VERTEX_BIT, sizeof(LightTransformData)});
        endConfig.uniformBuffers.push_back({0, sizeof(ShadowAssemblerConfig), VK_SHADER_STAGE_FRAGMENT_BIT});
        for (int i = 1; i <= 3; i++) {
            endConfig.samplers.push_back({i, VK_SHADER_STAGE_FRAGMENT_BIT});
        }
        std::vector<VkImageView> targetImages;
        targetImages.push_back(output->getView());
        endRenderPipeline = new VulkanEndRenderPipeline(device, syncManager, shader, &endConfig, width, height, targetImages, 1, output->getFormat());
        endRenderPipeline->getPushConstants()[0]->setData(&lightView);
        endRenderPipeline->getUniformBuffers()[0]->write(&shadowConfig);
    }
    void setNormalMapTexture(VulkanImage* normalMap){
        endRenderPipeline->getSamplers()[2]->setSamplerImageView(normalMap->getView());
    }
    void setPreviousAOImage(VulkanImage* previousAO){
        endRenderPipeline->getSamplers()[1]->setSamplerImageView(previousAO->getView());
    }
    void setShadowMap(VulkanImage* shadowMap){
        endRenderPipeline->getSamplers()[0]->setSamplerImageView(shadowMap->getView());
    }
    void updateSamplers() {
        endRenderPipeline->updateSamplers();
    }

};