//
// Created by Daniil on 03.04.2023.
//
#pragma once

#include "../../Vulkan/VulkanEndRenderPipeline.h"
#include "../../Util/ShaderLoader.h"
#include "../Camera/CameraManager.h"
#include "../../Vulkan/VulkanImage/VulkanImage.h"
#include "../GraphicalObjects/Material.h"

struct GBufferConfig {
    alignas(4) int combinedMetallicRoughness;
    alignas(4) int opacityMapEnabled;
    alignas(4) int emissiveEnabled;
    alignas(4) int aoEnabled;
};

class GBufferPipeline {
private:
    VulkanDevice *device;
    VulkanSyncManager *syncManager;
    VulkanShader *shader;
    VulkanEndRenderPipeline *endRenderPipeline;

    VulkanImage *positionsImage;
    VulkanImage *albedoMapImage;
    VulkanImage *normalMapImage;
    VulkanImage *metallicRoughnessEmissiveINVAO;
    VulkanImage* skyBoxSampled;
    GBufferConfig config{};
public:
    GBufferPipeline(VulkanDevice *device, unsigned int width, unsigned int height) : device(device) {
        syncManager = new VulkanSyncManager(device, nullptr);
        shader = ShaderLoader::loadShaders("shaders/GBufferPBR", device);
        PipelineEndConfig endConfig{};
        endConfig.vertexInputs.push_back({0, 3, sizeof(float), VK_FORMAT_R32G32B32_SFLOAT});
        endConfig.vertexInputs.push_back({1, 2, sizeof(float), VK_FORMAT_R32G32_SFLOAT});
        endConfig.vertexInputs.push_back({2, 3, sizeof(float), VK_FORMAT_R32G32B32_SFLOAT});

        endConfig.pushConstantInfos.push_back({VK_SHADER_STAGE_VERTEX_BIT, sizeof(PushConstantData)});
        endConfig.uniformBuffers.push_back({0, sizeof(GBufferConfig), VK_SHADER_STAGE_FRAGMENT_BIT});

        for (int i = 1; i <= 9; i++) {
            endConfig.samplers.push_back({i, VK_SHADER_STAGE_FRAGMENT_BIT});
        }

        positionsImage = VulkanImage::createImage(device, width, height);
        albedoMapImage = VulkanImage::createImage(device, width, height);
        normalMapImage = VulkanImage::createImage(device, width, height);
        skyBoxSampled = VulkanImage::createImage(device, width, height);
        metallicRoughnessEmissiveINVAO = VulkanImage::createImage(device, width, height);
        std::vector<VkImageView> renderTargets;
        renderTargets.push_back(positionsImage->getView());
        renderTargets.push_back(albedoMapImage->getView());
        renderTargets.push_back(normalMapImage->getView());
        renderTargets.push_back(metallicRoughnessEmissiveINVAO->getView());
        renderTargets.push_back(skyBoxSampled->getView());

        endRenderPipeline = new VulkanEndRenderPipeline(device, syncManager, shader, &endConfig, width, height,
                                                        renderTargets, 5, positionsImage->getFormat());
        endRenderPipeline->getUniformBuffers()[0]->write(&config);
        endRenderPipeline->updateUniforms();
    }

    void updateSamplers() {
        endRenderPipeline->updateSamplers();
    }

    void setWorldViewData(PushConstantData *data) {
        endRenderPipeline->getPushConstants()[0]->setData(data);
    }

    VkCommandBuffer beginRender() {
        endRenderPipeline->getUniformBuffers()[0]->write(&config);
        VkCommandBuffer cmd = endRenderPipeline->beginRender();

        endRenderPipeline->updatePcs();
        return cmd;
    }
    void setSkyBoxImage(VulkanImage* image){
        endRenderPipeline->getSamplers()[8]->setSamplerImageView(image->getView());
    }

    void bindImmediate(){
        endRenderPipeline->bindImmediate();
    }

    void endRender() {
        endRenderPipeline->endRender();
    }
    void populateSamplers(Material *material) {
        config.aoEnabled = material->getAoTexture() != nullptr;
        config.combinedMetallicRoughness = material->getMetallicRoughnessTexture() != nullptr;
        config.emissiveEnabled = material->getEmissiveTexture() != nullptr;
        config.opacityMapEnabled = material->getOpacityMapTexture() != nullptr;

        endRenderPipeline->getSamplers()[0]->setSamplerImageView(material->getAlbedoTexture()->getView());
        endRenderPipeline->getSamplers()[1]->setSamplerImageView(material->getNormalMap()->getView());
        if (config.combinedMetallicRoughness) {
            endRenderPipeline->getSamplers()[6]->setSamplerImageView(
                    material->getMetallicRoughnessTexture()->getView());
            endRenderPipeline->getSamplers()[2]->setSamplerImageView(material->getAlbedoTexture()->getView());
            endRenderPipeline->getSamplers()[3]->setSamplerImageView(material->getAlbedoTexture()->getView());
        } else {
            endRenderPipeline->getSamplers()[2]->setSamplerImageView(material->getMetallicTexture()->getView());
            endRenderPipeline->getSamplers()[3]->setSamplerImageView(material->getRoughnessTexture()->getView());
            endRenderPipeline->getSamplers()[6]->setSamplerImageView(material->getAlbedoTexture()->getView());
        }
        if (config.aoEnabled) {
            endRenderPipeline->getSamplers()[4]->setSamplerImageView(material->getAoTexture()->getView());
        } else {
            endRenderPipeline->getSamplers()[4]->setSamplerImageView(material->getAlbedoTexture()->getView());
        }
        if (config.emissiveEnabled) {
            endRenderPipeline->getSamplers()[5]->setSamplerImageView(material->getEmissiveTexture()->getView());
        } else {
            endRenderPipeline->getSamplers()[5]->setSamplerImageView(material->getAlbedoTexture()->getView());
        }
        if (config.opacityMapEnabled) {
            endRenderPipeline->getSamplers()[7]->setSamplerImageView(material->getOpacityMapTexture()->getView());
        } else {
            endRenderPipeline->getSamplers()[7]->setSamplerImageView(material->getAlbedoTexture()->getView());
        }
    }

    void resize(int width, int height) {
        vkDeviceWaitIdle(device->getDevice());
        delete positionsImage;
        delete albedoMapImage;
        delete normalMapImage;
        delete metallicRoughnessEmissiveINVAO;
        delete skyBoxSampled;
        positionsImage = VulkanImage::createImage(device, width, height);
        albedoMapImage = VulkanImage::createImage(device, width, height);
        normalMapImage = VulkanImage::createImage(device, width, height);
        skyBoxSampled = VulkanImage::createImage(device, width, height);
        metallicRoughnessEmissiveINVAO = VulkanImage::createImage(device, width, height);
        std::vector<VkImageView> renderTargets;
        renderTargets.push_back(positionsImage->getView());
        renderTargets.push_back(albedoMapImage->getView());
        renderTargets.push_back(normalMapImage->getView());
        renderTargets.push_back(metallicRoughnessEmissiveINVAO->getView());
        renderTargets.push_back(skyBoxSampled->getView());
        endRenderPipeline->resized(width, height, renderTargets, 5, positionsImage->getFormat());
        int i = 1+1;

    }

    VulkanImage *getPositionsImage() const {
        return positionsImage;
    }

    VulkanImage *getAlbedoMapImage() const {
        return albedoMapImage;
    }

    VulkanImage *getNormalMapImage() const {
        return normalMapImage;
    }

    VulkanImage *getMetallicRoughnessEmissiveInvao() const {
        return metallicRoughnessEmissiveINVAO;
    }

    VulkanImage *getSkyBoxSampled() {
        return skyBoxSampled;
    }
};