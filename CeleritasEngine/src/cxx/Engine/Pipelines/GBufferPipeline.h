//
// Created by Daniil on 30.04.2023.
//
#pragma once

#include "../RenderPipeline.h"
#include "../Camera/CameraManager.h"
#include "../GraphicalObjects/Material.h"
#include "../GraphicalObjects/Mesh.h"

struct GBufferConfig {
    alignas(4) int combinedMetallicRoughness;
    alignas(4) int opacityMapEnabled;
    alignas(4) int emissiveEnabled;
    alignas(4) int aoEnabled;
};


class GBufferPipeline : public RenderPipeline {
private:
    VulkanDevice *device;
    GBufferConfig config{};
    PushConstantData cameraData{};
    VkCommandBuffer currentCmd;
    std::map<Material *, VulkanDescriptorSet *> materialDescriptors;
    VulkanImage *skyBox = nullptr;
    PipelineEndConfig endConfig{};
public:
    GBufferPipeline(VulkanDevice *device, int width, int height) : RenderPipeline(device, nullptr), device(device) {

        endConfig.vertexInputs.push_back({0, 3, sizeof(float), VK_FORMAT_R32G32B32_SFLOAT});
        endConfig.vertexInputs.push_back({1, 2, sizeof(float), VK_FORMAT_R32G32_SFLOAT});
        endConfig.vertexInputs.push_back({2, 3, sizeof(float), VK_FORMAT_R32G32B32_SFLOAT});

        endConfig.pushConstantInfos.push_back({VK_SHADER_STAGE_VERTEX_BIT, sizeof(PushConstantData)});
        endConfig.uniformBuffers.push_back({0, sizeof(GBufferConfig), VK_SHADER_STAGE_FRAGMENT_BIT});
        endConfig.alphaBlend = true;
        for (int i = 1; i <= 9; i++) {
            endConfig.samplers.push_back({i, VK_SHADER_STAGE_FRAGMENT_BIT});
        }

        OutputConfig oc{};
        oc.imagePerStepAmount = 6;
        oc.amount = 6;
        oc.width = width;
        oc.height = height;
        RenderPipeline::initialize("shaders/GBufferPBR", endConfig, oc);
        RenderPipeline::getPushConstants()[0]->setData(&cameraData);
    }

    void beginRender() {
        currentCmd = RenderPipeline::beginRender();
    }

    void processMesh(Mesh *mesh) {
        prepareConfigForMaterial(mesh->getMaterial());
        VulkanDescriptorSet *materialDescriptorSet = materialDescriptors.at(mesh->getMaterial());
        if (materialDescriptorSet != nullptr) {
            RenderPipeline::bindImmediate(materialDescriptors.at(mesh->getMaterial()));
        } else {
            throw std::runtime_error("Error: you doesn't registered this material");
        }
        cameraData.worldMatrix = glm::mat4(1.0f);
        cameraData.worldMatrix = glm::translate(cameraData.worldMatrix, mesh->getPosition());
        cameraData.worldMatrix = glm::rotate(cameraData.worldMatrix, mesh->getRotation().x, glm::vec3(1, 0, 0));
        cameraData.worldMatrix = glm::rotate(cameraData.worldMatrix, mesh->getRotation().y, glm::vec3(0, 1, 0));
        cameraData.worldMatrix = glm::rotate(cameraData.worldMatrix, mesh->getRotation().z, glm::vec3(0, 0, 1));
        cameraData.worldMatrix = glm::scale(cameraData.worldMatrix, mesh->getScale());
        RenderPipeline::updatePushConstants();
        mesh->draw(currentCmd);
    }

    void endRender() {
        RenderPipeline::endRender();
    }

    GBufferConfig &getConfig()  {
        return config;
    }

    PushConstantData &getCameraData()  {
        return cameraData;
    }


    void setSkyBox(VulkanImage *skyBox) {
        this->skyBox = skyBox;
        for (const auto &item: materialDescriptors) {
            item.second->getSamplers()[8]->setSamplerImageView(skyBox->getView());
            item.second->updateDescriptorSet(0);
        }

    }

    void registerMaterial(Material *material) {
        VulkanDescriptorSet *descriptorSet = acquireDescriptorSet();
        descriptorSet->attachToObject(material);
        descriptorSet->getSamplers()[0]->setSamplerImageView(material->getAlbedoTexture()->getView());
        descriptorSet->getSamplers()[1]->setSamplerImageView(material->getNormalMap()->getView());
        if (material->getMetallicRoughnessTexture()) {
            descriptorSet->getSamplers()[6]->setSamplerImageView(
                    material->getMetallicRoughnessTexture()->getView());
            descriptorSet->getSamplers()[2]->setSamplerImageView(material->getAlbedoTexture()->getView());
            descriptorSet->getSamplers()[3]->setSamplerImageView(material->getAlbedoTexture()->getView());
        } else {
            descriptorSet->getSamplers()[2]->setSamplerImageView(material->getMetallicTexture()->getView());
            descriptorSet->getSamplers()[3]->setSamplerImageView(material->getRoughnessTexture()->getView());
            descriptorSet->getSamplers()[6]->setSamplerImageView(material->getAlbedoTexture()->getView());
        }
        if (material->getAoTexture()) {
            descriptorSet->getSamplers()[4]->setSamplerImageView(material->getAoTexture()->getView());
        } else {
            descriptorSet->getSamplers()[4]->setSamplerImageView(material->getAlbedoTexture()->getView());
        }
        if (material->getEmissiveTexture()) {
            descriptorSet->getSamplers()[5]->setSamplerImageView(material->getEmissiveTexture()->getView());
        } else {
            descriptorSet->getSamplers()[5]->setSamplerImageView(material->getAlbedoTexture()->getView());
        }
        if (material->getOpacityMapTexture()) {
            descriptorSet->getSamplers()[7]->setSamplerImageView(material->getOpacityMapTexture()->getView());
        } else {
            descriptorSet->getSamplers()[7]->setSamplerImageView(material->getAlbedoTexture()->getView());
        }

        descriptorSet->getSamplers()[8]->setSamplerImageView(skyBox->getView());

        descriptorSet->getUniformBuffers()[0]->write(&config);
        descriptorSet->updateDescriptorSet(0);
        materialDescriptors.insert(std::pair(material, descriptorSet));
    }

    void prepareConfigForMaterial(Material *material) {
        config.aoEnabled = material->getAoTexture() != nullptr;
        config.combinedMetallicRoughness = material->getMetallicRoughnessTexture() != nullptr;
        config.emissiveEnabled = material->getEmissiveTexture() != nullptr;
        config.opacityMapEnabled = material->getOpacityMapTexture() != nullptr;
        materialDescriptors.at(material)->getUniformBuffers()[0]->write(&config);
    }

};