#pragma once

#include "../RenderPipeline.h"
#include "../PrimitiveObjects/Quad.h"
#include <Vulkan/VulkanBuffers/VertexBuffer.h>
#include <Vulkan/VulkanBuffers/IndexBuffer.h>

struct UboData {
    int mode;
};

class AssemblyPipeline : public RenderPipeline {
private:
    VulkanDevice* device;
    VulkanSwapChain* swapChain;
    VertexBuffer *quadVBO;
    IndexBuffer *quadIBO;
    UboData data{};
    bool samplersUpdate = false;
public:
    AssemblyPipeline(VulkanDevice* device, VulkanSwapChain* swapChain, int width, int height) : RenderPipeline(device, swapChain), device(device), swapChain(swapChain){
        PipelineEndConfig endConfig{};
        endConfig.samplers.push_back({1, VK_SHADER_STAGE_FRAGMENT_BIT});
        endConfig.samplers.push_back({2, VK_SHADER_STAGE_FRAGMENT_BIT});

        endConfig.pushConstantInfos.push_back({VK_SHADER_STAGE_FRAGMENT_BIT, sizeof(UboData)});
        endConfig.vertexInputs.push_back({0, 3, sizeof(float), VK_FORMAT_R32G32B32_SFLOAT});
        endConfig.vertexInputs.push_back({1, 2, sizeof(float), VK_FORMAT_R32G32_SFLOAT});
        OutputConfig oc{};
        oc.amount = 0;
        oc.width = width;
        oc.height = height;
        oc.imagePerStepAmount = 1;
        RenderPipeline::initialize("shaders/OutputPipeline", endConfig, oc);
        quadVBO = new VertexBuffer(5 * sizeof(float), 4, device, quadVertices);
        quadIBO = new IndexBuffer(device, quadIndices, 6);
        RenderPipeline::getPushConstants()[0]->setData(&data);
    }
    void setGamePlaceHolder(VulkanImage* placeHolder){
        RenderPipeline::getSamplers()[1]->setSamplerImageView(placeHolder->getView());
        samplersUpdate = true;
    }

    void setGamePlaceHolder(VkImageView viewData){
        RenderPipeline::getSamplers()[1]->setSamplerImageView(viewData);
        samplersUpdate = true;
    }

    void setUiPlaceHolder(VulkanImage* placeHolder){
        RenderPipeline::getSamplers()[0]->setSamplerImageView(placeHolder->getView());
        samplersUpdate = true;
    }
    void update(){
        RenderPipeline::getPushConstants()[0]->setData(&data);
        VkCommandBuffer cmd = RenderPipeline::beginRender(samplersUpdate, false);

        samplersUpdate = false;
        quadVBO->bind(cmd);
        quadIBO->bind(cmd);
        quadIBO->draw(cmd);
        RenderPipeline::endRender();
    }
    UboData& getData(){
        return data;
    }
    void resize(int width, int height){
        RenderPipeline::resize(width, height);
    }
};