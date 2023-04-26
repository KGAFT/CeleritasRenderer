#pragma once

#include "../RenderPipeline.h"
#include "../PrimitiveObjects/Quad.h"
#include <Vulkan/VulkanBuffers/VertexBuffer.h>
#include <Vulkan/VulkanBuffers/IndexBuffer.h>
#include "../GraphicalObjects/Material.h"
#include "../GraphicalObjects/Mesh.h"
#include <glm/gtx/matrix_transform_2d.hpp>
struct LightViewData{
    glm::mat4 lightSpaceMatrix;
    glm::mat4 worldMatrix;
};

class ShadowBufferPipeline : public RenderPipeline{
private:
    VulkanDevice* device;
    LightViewData viewData{};
    VkCommandBuffer currentCmd;
public:
    ShadowBufferPipeline(VulkanDevice* device, int width, int height) : RenderPipeline(device, nullptr), device(device){
        PipelineEndConfig config{};
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
        currentCmd = RenderPipeline::beginRender(false, false);
        return currentCmd;
    }

    void processMesh(Mesh* mesh){
        viewData.worldMatrix = glm::mat4(1.0f);
    
        
        viewData.worldMatrix = glm::translate(viewData.worldMatrix, mesh->getPosition());
        viewData.worldMatrix = glm::rotate(viewData.worldMatrix, mesh->getRotation().x, glm::vec3(1,0,0));
        viewData.worldMatrix = glm::rotate(viewData.worldMatrix, mesh->getRotation().y, glm::vec3(0,1,0));
        viewData.worldMatrix = glm::rotate(viewData.worldMatrix, mesh->getRotation().z, glm::vec3(0,0,1));
        viewData.worldMatrix = glm::scale(viewData.worldMatrix, mesh->getScale());
        
       RenderPipeline::getPushConstants()[0]->setData(&viewData);
        RenderPipeline::updatePushConstants();
        mesh->draw(currentCmd);
    }

    void endRender(){
        RenderPipeline::endRender();
    }

     LightViewData &getViewData()  {
        return viewData;
    }
};