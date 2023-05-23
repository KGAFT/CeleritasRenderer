//
// Created by Daniil on 30.04.2023.
//
#pragma once
#include "../RenderPipeline.h"
#include <Vulkan/VulkanBuffers/VertexBuffer.h>
#include <Vulkan/VulkanBuffers/IndexBuffer.h>
#include "../PrimitiveObjects/Quad.h"

struct UboData {
    int mode;
};


class AssemblyPipeline : public RenderPipeline{
private:
    VulkanDevice* device;
    VulkanSwapChain* swapChain;
    VertexBuffer *quadVBO;
    IndexBuffer *quadIBO;
    UboData data{};
    VulkanDescriptorSet* descriptorSet;
    PipelineEndConfig endConfig{};
public:
    AssemblyPipeline(VulkanDevice *device, VulkanSwapChain *swapChain, int width, int height) : RenderPipeline(device, swapChain), device(device),
                                                    swapChain(swapChain) {

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
        quadVBO = new VertexBuffer(5 * sizeof(float), 4, device, QuadVertices);
        quadIBO = new IndexBuffer(device, QuadIndices, 6);
        RenderPipeline::getPushConstants()[0]->setData(&data);
        descriptorSet = RenderPipeline::acquireDescriptorSet();
        descriptorSet->attachToObject(this);
    }
    void setGamePlaceHolder(VulkanImage* placeHolder){
        descriptorSet->getSamplers()[1]->setSamplerImageView(placeHolder->getView());
    }

    void setUiPlaceHolder(VulkanImage* placeHolder){
        descriptorSet->getSamplers()[0]->setSamplerImageView(placeHolder->getView());
    }

    void confirmPlaceHolders(){
        for(int i = 0; i<3; i++){
            descriptorSet->updateDescriptorSet(i);
        }

    }

    void update(){
        RenderPipeline::getPushConstants()[0]->setData(&data);
        VkCommandBuffer cmd = RenderPipeline::beginRender();
        bindImmediate(descriptorSet);
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