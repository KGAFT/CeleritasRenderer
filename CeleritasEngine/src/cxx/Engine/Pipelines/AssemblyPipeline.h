#pragma once

#include "../../Vulkan/VulkanEndRenderPipeline.h"
#include "../../Util/ShaderLoader.h"

struct UboData {
    int mode;
};

float quadVertices[]{
        -1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
        -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
        1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
        1.0f, -1.0f, 0.0f, 1.0f, 0.0f};

unsigned int indices[]{
        0, 1, 2,
        3, 2, 1};

class AssemblyPipeline {
private:
    VulkanEndRenderPipeline *endRenderPipeline;
    VulkanDevice *device;
    VulkanSyncManager *syncManager;
    VulkanShader *shader;
    UboData data{};
    VulkanImage *gamePlaceHolder;
    VulkanImage *uiPlaceHolder;

    VertexBuffer *quadVBO;
    IndexBuffer *quadIBO;

public:
    AssemblyPipeline(VulkanDevice *device, VulkanSwapChain *swapChain, Window *window) {
        shader = ShaderLoader::loadShaders("shaders/OutputPipeline", device);
        syncManager = new VulkanSyncManager(device, swapChain);
        PipelineEndConfig endConfig{};
        endConfig.samplers.push_back({1, VK_SHADER_STAGE_FRAGMENT_BIT});
        endConfig.samplers.push_back({2, VK_SHADER_STAGE_FRAGMENT_BIT});

        endConfig.pushConstantInfos.push_back({VK_SHADER_STAGE_FRAGMENT_BIT, sizeof(UboData)});
        endConfig.vertexInputs.push_back({0, 3, sizeof(float), VK_FORMAT_R32G32B32_SFLOAT});
        endConfig.vertexInputs.push_back({1, 2, sizeof(float), VK_FORMAT_R32G32_SFLOAT});
        endRenderPipeline = new VulkanEndRenderPipeline(device, syncManager, shader, &endConfig, window->getWidth(),
                                                        window->getHeight(), swapChain->getSwapChainImageViews(), 1,
                                                        swapChain->getSwapChainImageFormat());
        data.mode = 0;
        quadVBO = new VertexBuffer(5 * sizeof(float), 4, device, quadVertices);
        quadIBO = new IndexBuffer(device, indices, 6);
        endRenderPipeline->getPushConstants()[0]->setData(&data);
    }

    void prepare() {
        updateSamplers();

        //endRenderPipeline->updateUniforms();
    }

    void updateSamplers() {
        endRenderPipeline->getSamplers()[0]->setSamplerImageView(uiPlaceHolder->getView());
        endRenderPipeline->getSamplers()[1]->setSamplerImageView(gamePlaceHolder->getView());
        endRenderPipeline->updateSamplers();
    }

    void update() {
        endRenderPipeline->getPushConstants()[0]->setData(&data);

        VkCommandBuffer cmd = endRenderPipeline->beginRender();
        endRenderPipeline->bindImmediate();
        endRenderPipeline->updatePcs();
        quadVBO->bind(cmd);
        quadIBO->bind(cmd);
        quadIBO->draw(cmd);
        endRenderPipeline->endRender();
    }

    void SetGamePlaceHolder(VulkanImage *gamePlaceHolder) {
        this->gamePlaceHolder = gamePlaceHolder;
    }

    void SetUiPlaceHolder(VulkanImage *uiPlaceHolder) {
        this->uiPlaceHolder = uiPlaceHolder;
    }

    UboData &getData() {
        return data;
    }
};