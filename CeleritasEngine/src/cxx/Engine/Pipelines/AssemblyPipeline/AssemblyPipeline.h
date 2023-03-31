//
// Created by KGAFT on 3/18/2023.
//

#pragma once

#include "../../../Vulkan/VulkanDevice/VulkanDevice.h"
#include "../../../Vulkan/VulkanSwapChain.h"
#include "../../../Vulkan/VulkanSync/VulkanSyncManager.h"
#include "../../../Vulkan/VulkanEndRenderPipeline.h"
#include "../../../Util/ShaderLoader.h"
#include "glm/vec4.hpp"
#include "../../../Vulkan/VulkanBuffers/VertexBuffer.h"
#include "../../../Vulkan/VulkanBuffers/IndexBuffer.h"

struct ColorCorrect{
    glm::vec4 uiCorrectAmplifier;
    glm::vec4 gameCorrectAmplifier;

    glm::vec4 uiCorrectAdder;
    glm::vec4 gameCorrectAdder;

    glm::vec4 outCorrectAmplifier;
    glm::vec4 outCorrectAdder;
};

float quadVertices[]{
        -1.0f,  1.0f, 0.0f,     0.0f, 1.0f,
        -1.0f, -1.0f, 0.0f,     0.0f, 0.0f,
        1.0f,  1.0f, 0.0f,      1.0f, 1.0f,
        1.0f, -1.0f, 0.0f,      1.0f, 0.0f
};

unsigned int indices[]{
        0, 1, 2,
        3, 2, 1
};


class AssemblyPipeline : public IWindowResizeCallback{
private:
    VulkanDevice* device;
    VulkanSwapChain* swapChain;
    VulkanSyncManager* syncManager;
    VulkanEndRenderPipeline* endRenderPipeline;
    VulkanShader* shader;
    Window* window;
    ColorCorrect correct;
    VertexBuffer* quadVBO;
    IndexBuffer* quadIBO;
public:
    AssemblyPipeline(VulkanDevice *device, VulkanSwapChain *swapChain, Window* window) : device(device), swapChain(swapChain) {
        shader = ShaderLoader::loadShaders("shaders/OutputPipeline", device);
        syncManager = new VulkanSyncManager(device, swapChain);

        PipelineEndConfig endConfig{};
        endConfig.samplers.push_back({1, VK_SHADER_STAGE_FRAGMENT_BIT});
        endConfig.samplers.push_back({2, VK_SHADER_STAGE_FRAGMENT_BIT});

        endConfig.uniformBuffers.push_back({0, sizeof(ColorCorrect), VK_SHADER_STAGE_FRAGMENT_BIT});

        endConfig.vertexInputs.push_back({0, 3, sizeof(float), VK_FORMAT_R32G32B32_SFLOAT});
        endConfig.vertexInputs.push_back({1, 2, sizeof(float), VK_FORMAT_R32G32_SFLOAT});

        endRenderPipeline = new VulkanEndRenderPipeline(device, syncManager, shader, &endConfig, window->getWidth(), window->getHeight(), swapChain->getSwapChainImageViews(), 1, swapChain->getSwapChainImageFormat());
        window->registerResizeCallback(this);

        quadVBO = new VertexBuffer(5*sizeof(float), 4, device, quadVertices);
        quadIBO = new IndexBuffer(device, indices, 6);
    }

    void update(){
        endRenderPipeline->getUniformBuffers()[0]->write(&correct);
        endRenderPipeline->updateUniforms();
        VkCommandBuffer cmd = endRenderPipeline->beginRender();
        quadVBO->bind(cmd);
        quadIBO->bind(cmd);
        quadIBO->draw(cmd);
        endRenderPipeline->endRender();
    }

    ColorCorrect &getCorrect() {
        return correct;
    }

    VulkanSampler* getUISampler(){
        return endRenderPipeline->getSamplers()[0];
    }
    VulkanSampler* getGameSampler(){
        return endRenderPipeline->getSamplers()[1];
    }

    void resized(int width, int height) override {
        endRenderPipeline->resized(width, height);
    }
};