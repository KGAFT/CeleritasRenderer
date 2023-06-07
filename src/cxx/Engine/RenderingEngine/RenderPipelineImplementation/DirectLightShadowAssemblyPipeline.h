//
// Created by Daniil on 01.05.2023.
//
#pragma once

#include "../RenderPipeline.h"
#include "../PrimitiveObjects/Quad.h"
#include "Vulkan/VulkanBuffers/VertexBuffer.h"
#include "Vulkan/VulkanBuffers/IndexBuffer.h"
#include "../GraphicalObjects/Material.h"
#include "../GraphicalObjects/Mesh.h"
#include "glm/gtc/matrix_transform.hpp"


namespace RenderingEngine{
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


    class DirectLightShadowAssemblyPipeline : public RenderPipeline
    {
    public:
        DirectLightShadowAssemblyPipeline(VulkanDevice* device, int width, int height) : RenderPipeline(device, nullptr), device(device){

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
        }
    private:
        VulkanDevice *device;
        ShadowAssemblerConfig config{};
        WorldTransformData wtData{};
        VkCommandBuffer currentCmd;
        std::map<Material *, VulkanDescriptorSet *> materialDescriptors;
        VulkanImage* shadowMap;
        PipelineEndConfig endConfig{};

    public:
        VkCommandBuffer beginRender(){
            currentCmd = RenderPipeline::beginRender();
            return currentCmd;
        }
        void processMesh(Mesh* mesh){
            wtData.worldMatrix = mesh->getWorldMatrix();
            VulkanDescriptorSet* descriptorSet = populateConfigForMaterial(mesh->getMaterial());
            RenderPipeline::bindImmediate(descriptorSet);
            RenderPipeline::updatePushConstants();
            mesh->draw(currentCmd);
        }

        void endRenderPass(){
            RenderPipeline::endRenderPass();
        }

        VulkanDescriptorSet* populateConfigForMaterial(Material* material){
            VulkanDescriptorSet* descriptorSet = materialDescriptors.at(material);
            config.normalMapEnabled = material->getNormalMap()!=nullptr;
            config.previousAoEnabled = material->getAoTexture()!=nullptr;
            if(descriptorSet!=nullptr){
                descriptorSet->getUniformBuffers()[0]->write(&config);
            }
            else{
                throw std::runtime_error("Error: you trying to use unregistered material");
            }
            return descriptorSet;
        }

        void setShadowMap(VulkanImage* shadowMap){
            this->shadowMap = shadowMap;
            for (auto &item: materialDescriptors){
                item.second->getSamplers()[0]->setSamplerImageView(shadowMap->getView());
                item.second->updateDescriptorSet(0);
            }
        }



        void registerMeshMaterial(Material* material){
            if(materialDescriptors.count(material)==0){
                VulkanDescriptorSet* descriptorSet = RenderPipeline::acquireDescriptorSet();
                descriptorSet->attachToObject(material);
                if(material->getAoTexture()!=nullptr) {
                    descriptorSet->getSamplers()[1]->setSamplerImageView(material->getAoTexture()->getView());
                }
                if(material->getNormalMap()!=nullptr){
                    descriptorSet->getSamplers()[2]->setSamplerImageView(material->getNormalMap()->getView());
                }
                if(shadowMap!=nullptr){
                    descriptorSet->getSamplers()[0]->setSamplerImageView(shadowMap->getView());
                }
                descriptorSet->getUniformBuffers()[0]->write(&config);
                descriptorSet->updateDescriptorSet(0);
                materialDescriptors.insert(std::pair(material, descriptorSet));
            }
        }

        void unRegisterMeshMaterial(Material* material){
            if(materialDescriptors.count(material)>0){
                materialDescriptors.at(material)->attachToObject(nullptr);
                materialDescriptors.erase(material);
            }
        }

        ShadowAssemblerConfig &getConfig() {
            return config;
        }

        WorldTransformData &getWorldTransfomData() {
            return wtData;
        }

    };
}
