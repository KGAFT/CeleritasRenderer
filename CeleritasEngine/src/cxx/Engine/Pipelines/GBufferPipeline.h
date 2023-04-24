//
// Created by Daniil on 24.04.2023.
//
#pragma once

#include "../RenderPipeline.h"
#include "../PrimitiveObjects/Quad.h"
#include <Vulkan/VulkanBuffers/VertexBuffer.h>
#include <Vulkan/VulkanBuffers/IndexBuffer.h>
#include "../Camera/CameraManager.h"
#include "../GraphicalObjects/Material.h"
#include "../GraphicalObjects/mesh.h"

struct GBufferConfig {
    alignas(4) int combinedMetallicRoughness;
    alignas(4) int opacityMapEnabled;
    alignas(4) int emissiveEnabled;
    alignas(4) int aoEnabled;
};

class GBufferPipeline : public RenderPipeline{
private:
    VulkanDevice* device;
    GBufferConfig config{};
    PushConstantData pcData{};
    VkCommandBuffer  currentCmd;
public:
    GBufferPipeline(VulkanDevice* device, int width, int height) : RenderPipeline(device, nullptr), device(device){
        PipelineEndConfig endConfig{};
        endConfig.vertexInputs.push_back({0, 3, sizeof(float), VK_FORMAT_R32G32B32_SFLOAT});
        endConfig.vertexInputs.push_back({1, 2, sizeof(float), VK_FORMAT_R32G32_SFLOAT});
        endConfig.vertexInputs.push_back({2, 3, sizeof(float), VK_FORMAT_R32G32B32_SFLOAT});

        endConfig.pushConstantInfos.push_back({VK_SHADER_STAGE_VERTEX_BIT, sizeof(PushConstantData)});
        endConfig.uniformBuffers.push_back({0, sizeof(GBufferConfig), VK_SHADER_STAGE_FRAGMENT_BIT});

        for (int i = 1; i <= 9; i++) {
            endConfig.samplers.push_back({i, VK_SHADER_STAGE_FRAGMENT_BIT});
        }

        OutputConfig oc{};
        oc.imagePerStepAmount = 6;
        oc.amount = 6;
        oc.width = width;
        oc.height = height;
        RenderPipeline::initialize("shaders/GBufferPBR", endConfig,oc);
        RenderPipeline::getPushConstants()[0]->setData(&pcData);
        RenderPipeline::getUniforms()[0]->write(&config);
        RenderPipeline::updateUniforms();
    }

    void beginRender(){
        currentCmd = RenderPipeline::beginRender(false, false);
    }
    void processMesh(Mesh* mesh){
        populateSamplers(mesh->getMaterial());
        RenderPipeline::getUniforms()[0]->write(&config);
        pcData.worldMatrix = glm::mat4(1.0f);
        pcData.worldMatrix = glm::translate(pcData.worldMatrix, mesh->getPosition());
        pcData.worldMatrix = glm::rotate(pcData.worldMatrix, mesh->getRotation().x, glm::vec3(1,0,0));
        pcData.worldMatrix = glm::rotate(pcData.worldMatrix, mesh->getRotation().y, glm::vec3(0,1,0));
        pcData.worldMatrix = glm::rotate(pcData.worldMatrix, mesh->getRotation().z, glm::vec3(0,0,1));
        pcData.worldMatrix = glm::scale(pcData.worldMatrix, mesh->getScale());
        RenderPipeline::updatePushConstants();
        mesh->draw(currentCmd);
    }

    void setSkyBox(VulkanImage* skyBox){
        RenderPipeline::getSamplers()[8]->setSamplerImageView(skyBox->getView());
    }

    void endRender(){
        RenderPipeline::endRender();
    }

     GBufferConfig &getConfig()  {
        return config;
    }

     PushConstantData &getPcData()  {
        return pcData;
    }

    void populateSamplers(Material *material) {
        config.aoEnabled = material->getAoTexture() != nullptr;
        config.combinedMetallicRoughness = material->getMetallicRoughnessTexture() != nullptr;
        config.emissiveEnabled = material->getEmissiveTexture() != nullptr;
        config.opacityMapEnabled = material->getOpacityMapTexture() != nullptr;

        RenderPipeline::getSamplers()[0]->setSamplerImageView(material->getAlbedoTexture()->getView());
        RenderPipeline::getSamplers()[1]->setSamplerImageView(material->getNormalMap()->getView());
        if (config.combinedMetallicRoughness) {
            RenderPipeline::getSamplers()[6]->setSamplerImageView(
                    material->getMetallicRoughnessTexture()->getView());
            RenderPipeline::getSamplers()[2]->setSamplerImageView(material->getAlbedoTexture()->getView());
            RenderPipeline::getSamplers()[3]->setSamplerImageView(material->getAlbedoTexture()->getView());
        } else {
            RenderPipeline::getSamplers()[2]->setSamplerImageView(material->getMetallicTexture()->getView());
            RenderPipeline::getSamplers()[3]->setSamplerImageView(material->getRoughnessTexture()->getView());
            RenderPipeline::getSamplers()[6]->setSamplerImageView(material->getAlbedoTexture()->getView());
        }
        if (config.aoEnabled) {
            RenderPipeline::getSamplers()[4]->setSamplerImageView(material->getAoTexture()->getView());
        } else {
            RenderPipeline::getSamplers()[4]->setSamplerImageView(material->getAlbedoTexture()->getView());
        }
        if (config.emissiveEnabled) {
            RenderPipeline::getSamplers()[5]->setSamplerImageView(material->getEmissiveTexture()->getView());
        } else {
            RenderPipeline::getSamplers()[5]->setSamplerImageView(material->getAlbedoTexture()->getView());
        }
        if (config.opacityMapEnabled) {
            RenderPipeline::getSamplers()[7]->setSamplerImageView(material->getOpacityMapTexture()->getView());
        } else {
            RenderPipeline::getSamplers()[7]->setSamplerImageView(material->getAlbedoTexture()->getView());
        }
        RenderPipeline::updateSamplers();
    }

};
