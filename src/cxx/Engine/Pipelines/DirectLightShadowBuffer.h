//
// Created by Daniil on 01.05.2023.
//

#pragma once

#include "../RenderPipeline.h"
#include "../PrimitiveObjects/Quad.h"
#include <Vulkan/VulkanBuffers/VertexBuffer.h>
#include <Vulkan/VulkanBuffers/IndexBuffer.h>
#include "../GraphicalObjects/Material.h"
#include "../GraphicalObjects/Mesh.h"
#include <glm/gtc/matrix_transform.hpp>

struct LightViewData{
    glm::mat4 lightSpaceMatrix;
    glm::mat4 worldMatrix;
};


class DirectLightShadowBuffer : public RenderPipeline{
private:
    VulkanDevice* device;
    LightViewData viewData{};
    VkCommandBuffer currentCmd;
    PipelineEndConfig config{};
public:
    DirectLightShadowBuffer(VulkanDevice* device, int width, int height) :  RenderPipeline(device, nullptr), device(device){

            config.vertexInputs.push_back({0, 3, sizeof(float), VK_FORMAT_R32G32B32_SFLOAT});
            config.vertexInputs.push_back({1, 2, sizeof(float), VK_FORMAT_R32G32_SFLOAT});
            config.vertexInputs.push_back({2, 3, sizeof(float), VK_FORMAT_R32G32B32_SFLOAT});
            config.pushConstantInfos.push_back({VK_SHADER_STAGE_VERTEX_BIT, sizeof(LightViewData)});
            OutputConfig oc{};
            oc.amount = 1;
            oc.width = width;
            oc.height = height;
            oc.imagePerStepAmount = 1;
            RenderPipeline::initialize("shaders/DirectLightShadowBuffer", config, oc);
            RenderPipeline::getPushConstants()[0]->setData(&viewData);
    }

    void recalculateMatrixForLightSource(glm::vec3 lightPos, int maxLightShadowDistance){
        glm::mat4 lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 0.1f, (float)maxLightShadowDistance);
        glm::mat4 lightView = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
        viewData.lightSpaceMatrix = lightProjection*lightView;
    }

    VkCommandBuffer beginRender(){
        currentCmd = RenderPipeline::beginRender();
        return currentCmd;
    }

    void processMesh(Mesh* mesh){
        viewData.worldMatrix = mesh->getWorldMatrix();
        RenderPipeline::getPushConstants()[0]->setData(&viewData);
        RenderPipeline::updatePushConstants();
        mesh->draw(currentCmd);
    }

     LightViewData &getViewData() {
        return viewData;
    }

    void endRender(){
        RenderPipeline::endRender();
    }

};