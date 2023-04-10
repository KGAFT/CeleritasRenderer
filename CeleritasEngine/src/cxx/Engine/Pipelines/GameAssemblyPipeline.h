//
// Created by Daniil on 10.04.2023.
//

#pragma once

#include "../../Vulkan/VulkanEndRenderPipeline.h"
#include "../../Util/ShaderLoader.h"
#include "../../Vulkan/VulkanImage/VulkanImage.h"
#include "GBufferPipeline.h"
#include "../../Vulkan/VulkanBuffers/VertexBuffer.h"
#include "../../Vulkan/VulkanBuffers/IndexBuffer.h"
#include "../PrimitiveObjects/Quad.h"
#define LIGHT_BLOCKS_AMOUNT 100




struct PointLight {
    alignas(16) glm::vec3 position;
    alignas(16) glm::vec3 color;
    alignas(4) float intensity;
};

struct DirectLight {
    alignas(16) glm::vec3 direction;
    alignas(16) glm::vec3 color;
    alignas(4) float intensity;
};


struct LightConfiguration {
    DirectLight directLights[LIGHT_BLOCKS_AMOUNT];
    PointLight pointLights[LIGHT_BLOCKS_AMOUNT];
    alignas(4) int enabledDirects = 0;
    alignas(4) int enabledPoints = 0;


    alignas(4) float emissiveIntensity = 2;
    alignas(4) float emissiveShininess = 1;
    alignas(4) float gammaCorrect = 1.0f / 2.2f;
    alignas(4) float ambientIntensity = 0.03f;
};

struct VertexConfig {
     glm::vec3 cameraPosition = glm::vec3(0);
};

class GameAssemblyPipeline {
private:
    VulkanDevice *device;
    VulkanShader *shader;
    VulkanEndRenderPipeline *endRenderPipeline;
    VulkanSyncManager *syncManager;
    VulkanImage *output;
    LightConfiguration config{};
    GBufferPipeline *gBufferPipeline;

    VertexConfig* vertexConfig = new VertexConfig{};
    VertexBuffer *quadVBO;
    IndexBuffer *quadIBO;
public:
    GameAssemblyPipeline(VulkanDevice *device, int width, int height) : device(device) {
        syncManager = new VulkanSyncManager(device, nullptr);
        shader = ShaderLoader::loadShaders("shaders/GameAssemblyPipeline", device);
        PipelineEndConfig endConfig{};
        endConfig.vertexInputs.push_back({0, 3, sizeof(float), VK_FORMAT_R32G32B32_SFLOAT});
        endConfig.vertexInputs.push_back({1, 2, sizeof(float), VK_FORMAT_R32G32_SFLOAT});
        endConfig.pushConstantInfos.push_back({VK_SHADER_STAGE_VERTEX_BIT, sizeof(VertexConfig)});
        endConfig.uniformBuffers.push_back({0, sizeof(LightConfiguration), VK_SHADER_STAGE_FRAGMENT_BIT});
        for (int i = 1; i <= 4; i++) {
            endConfig.samplers.push_back({i, VK_SHADER_STAGE_FRAGMENT_BIT});
        }
        output = VulkanImage::createImage(device, width, height);
        std::vector<VkImageView> outputImages;
        outputImages.push_back(output->getView());
        endRenderPipeline = new VulkanEndRenderPipeline(device, syncManager, shader, &endConfig, width, height,
                                                        outputImages, 1, output->getFormat());
        endRenderPipeline->getUniformBuffers()[0]->write(&config);
        endRenderPipeline->updateUniforms();
        quadVBO = new VertexBuffer(5 * sizeof(float), 4, device, quadVertices);
        quadIBO = new IndexBuffer(device, quadIndices, 6);
    }

    GBufferPipeline *getGBufferPipeline() const {
        return gBufferPipeline;
    }

    void setGBufferPipeline(GBufferPipeline *gBufferPipeline) {
        GameAssemblyPipeline::gBufferPipeline = gBufferPipeline;
        endRenderPipeline->getSamplers()[0]->setSamplerImageView(gBufferPipeline->getPositionsImage()->getView());
        endRenderPipeline->getSamplers()[1]->setSamplerImageView(gBufferPipeline->getAlbedoMapImage()->getView());
        endRenderPipeline->getSamplers()[2]->setSamplerImageView(gBufferPipeline->getNormalMapImage()->getView());
        endRenderPipeline->getSamplers()[3]->setSamplerImageView(
                gBufferPipeline->getMetallicRoughnessEmissiveInvao()->getView());
        endRenderPipeline->updateSamplers();

    }

    void update() {
        endRenderPipeline->getPushConstants()[0]->setData(vertexConfig);
        endRenderPipeline->getUniformBuffers()[0]->write(&config);
        VkCommandBuffer  cmd = endRenderPipeline->beginRender();
        endRenderPipeline->bindImmediate();
        endRenderPipeline->updatePcs();
        quadVBO->bind(cmd);
        quadIBO->bind(cmd);
        quadIBO->draw(cmd);
        endRenderPipeline->endRender();
    }

    LightConfiguration &getConfig() {
        return config;
    }

    VulkanImage *getOutput() {
        return output;
    }

    VertexConfig *getVertexConfig() {
        return vertexConfig;
    }
};