#pragma once

#include "../RenderPipeline.h"
#include "../PrimitiveObjects/Quad.h"
#include <Vulkan/VulkanBuffers/VertexBuffer.h>
#include <Vulkan/VulkanBuffers/IndexBuffer.h>
#include "../GraphicalObjects/Material.h"
#include "../GraphicalObjects/Mesh.h"
#include <glm/gtx/matrix_transform_2d.hpp>

struct ShadowAssemblerConfig
{
    alignas(16) glm::vec3 lightPosition;
    alignas(4) int normalMapEnabled;
    alignas(4) int previousAoEnabled;
};

struct WorldTransformData
{
    glm::mat4 lightSpaceMatrix;
    glm::mat4 worldMatrix;
    glm::mat4 viewMatrix;
    glm::vec3 cameraPosition;
};

class ShadowAssemblyPipeline : public RenderPipeline
{
private:
    VulkanDevice *device;
    ShadowAssemblerConfig config{};
    WorldTransformData wtData{};
    VkCommandBuffer currentCmd;

public:
    ShadowAssemblyPipeline(VulkanDevice* device, int width, int height) : RenderPipeline(device, nullptr), device(device){
        PipelineEndConfig endConfig{};
        endConfig.vertexInputs.push_back({0, 3, sizeof(float), VK_FORMAT_R32G32B32_SFLOAT});
        endConfig.vertexInputs.push_back({1, 2, sizeof(float), VK_FORMAT_R32G32_SFLOAT});
        endConfig.vertexInputs.push_back({2, 3, sizeof(float), VK_FORMAT_R32G32B32_SFLOAT});
        endConfig.pushConstantInfos.push_back({VK_SHADER_STAGE_VERTEX_BIT, sizeof(WorldTransformData)});
        endConfig.uniformBuffers.push_back({0, sizeof(ShadowAssemblerConfig), VK_SHADER_STAGE_FRAGMENT_BIT});
        for (int i = 1; i <= 3; i++) {
            endConfig.samplers.push_back({i, VK_SHADER_STAGE_FRAGMENT_BIT});
        }
        OutputConfig oc{};
        oc.amount = 1;
        oc.width = width;
        oc.height = height;
        oc.imagePerStepAmount = 1;
        RenderPipeline::initialize("shaders/ShadowAssemblePipeline", endConfig, oc);
        RenderPipeline::getPushConstants()[0]->setData(&wtData);
        RenderPipeline::getUniforms()[0]->write(&config);
        RenderPipeline::updateUniforms();
    }
    VkCommandBuffer beginRender(){
        currentCmd = RenderPipeline::beginRender(false, false);
        return currentCmd;
    }
    void processMesh(Mesh* mesh, bool calculateWorldMatrix){
        if(calculateWorldMatrix){
            wtData.worldMatrix = glm::mat4(1.0f);
            wtData.worldMatrix = glm::translate(wtData.worldMatrix, mesh->getPosition());
            wtData.worldMatrix = glm::rotate(wtData.worldMatrix, mesh->getRotation().x, glm::vec3(1,0,0));
            wtData.worldMatrix = glm::rotate(wtData.worldMatrix, mesh->getRotation().y, glm::vec3(0,1,0));
            wtData.worldMatrix = glm::rotate(wtData.worldMatrix, mesh->getRotation().z, glm::vec3(0,0,1));
            wtData.worldMatrix = glm::scale(wtData.worldMatrix, mesh->getScale());
        }
        RenderPipeline::getUniforms()[0]->write(&config);
        RenderPipeline::updatePushConstants();
        mesh->draw(currentCmd);
    }

    void endRender(){
        RenderPipeline::endRender();
    }

    void setPreviousAo(VulkanImage* ao){
        if(ao!=nullptr){
            RenderPipeline::getSamplers()[1]->setSamplerImageView(ao->getView());
        }

    }
    void setShadowMap(VkImageView shadowMap){
        RenderPipeline::getSamplers()[0]->setSamplerImageView(shadowMap);
    }
    void setNormalMap(VulkanImage* normalMap){
        if(normalMap!=nullptr){
            RenderPipeline::getSamplers()[2]->setSamplerImageView(normalMap->getView());
        }

    }
    void updateSamplers(){
        RenderPipeline::updateSamplers();
    }
    ShadowAssemblerConfig& getConfig(){
        return config;
    }
    WorldTransformData& getWorldTransfomData(){
        return wtData;
    }
};
