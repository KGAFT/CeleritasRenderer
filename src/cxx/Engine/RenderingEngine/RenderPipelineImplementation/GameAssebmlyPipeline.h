//
// Created by Daniil on 01.05.2023.
//
#pragma once

#define LIGHT_BLOCKS_AMOUNT 100

#include "../RenderPipeline.h"
#include "../PrimitiveObjects/Quad.h"
#include "Vulkan/VulkanBuffers/VertexBuffer.h"
#include "Vulkan/VulkanBuffers/IndexBuffer.h"
#include "Vulkan/VulkanImage/VulkanCubemapImage.h"
#include "../Camera/CameraManager.h"
#include "../GraphicalObjects/Material.h"
#include "../GraphicalObjects/Mesh.h"
#include "GBufferPipeline.h"
#include <glm/glm.hpp>

namespace RenderingEngine{

    struct PointLight {
        alignas(16) glm::vec3 position;
        alignas(16) glm::vec3 color;
        alignas(4) float intensity;
    };

    struct DirectLight {
        alignas(16) glm::vec3 direction;
        alignas(16) glm::vec3 color;
        alignas(4) float intensity;
    };


    struct LightConfiguration {
        DirectLight directLights[LIGHT_BLOCKS_AMOUNT];
        PointLight pointLights[LIGHT_BLOCKS_AMOUNT];
        alignas(4) int enabledDirects = 0;
        alignas(4) int enabledPoints = 0;


        alignas(4) float emissiveIntensity = 2;
        alignas(4) float emissiveShininess = 1;
        alignas(4) float gammaCorrect = 1.0f / 2.2f;
        alignas(4) float ambientIntensity = 0.03f;
    };

    struct VertexConfig {
        glm::vec3 cameraPosition = glm::vec3(0);
    };

    class GameAssemblyPipeline  : public RenderPipeline{
    public:
        GameAssemblyPipeline(VulkanDevice* device, int width, int height) : RenderPipeline(device, nullptr), device(device){
            PipelineEndConfig endConfig{};
            endConfig.vertexInputs.push_back({0, 3, sizeof(float), VK_FORMAT_R32G32B32_SFLOAT});
            endConfig.vertexInputs.push_back({1, 2, sizeof(float), VK_FORMAT_R32G32_SFLOAT});
            endConfig.pushConstantInfos.push_back({VK_SHADER_STAGE_VERTEX_BIT, sizeof(VertexConfig)});
            endConfig.uniformBuffers.push_back({0, sizeof(LightConfiguration), VK_SHADER_STAGE_FRAGMENT_BIT});
            for (int i = 1; i <= 6; i++) {
                endConfig.samplers.push_back({i, VK_SHADER_STAGE_FRAGMENT_BIT});
            }
            OutputConfig oc{};
            oc.width = width;
            oc.height = height;
            oc.amount = 1;
            oc.imagePerStepAmount = 1;
            RenderPipeline::initialize("shaders/GameAssemblyPipeline", endConfig, oc);
            quadVBO = new VertexBuffer(5 * sizeof(float), 4, device, QuadVertices);
            quadIBO = new IndexBuffer(device, QuadIndices, 6);

            RenderPipeline::getPushConstants()[0]->setData(&vertexConfig);
            gBufferDescriptorSet = RenderPipeline::acquireDescriptorSet();
            gBufferDescriptorSet->getUniformBuffers()[0]->write(&lightConfig);
            gBufferDescriptorSet->attachToObject(this);
        }
    private:
        VulkanDevice* device;
        VertexBuffer *quadVBO;
        IndexBuffer *quadIBO;
        GBufferPipeline* gBufferPipeline;
        LightConfiguration lightConfig{};
        VertexConfig vertexConfig{};
        VulkanDescriptorSet* gBufferDescriptorSet;

    public:
        void update(){
            if(gBufferPipeline!=nullptr){
                gBufferDescriptorSet->getUniformBuffers()[0]->write(&lightConfig);
                RenderPipeline::getPushConstants()[0]->setData(&vertexConfig);
                VkCommandBuffer cmd = RenderPipeline::beginRender();
                RenderPipeline::bindImmediate(gBufferDescriptorSet);
                quadVBO->bind(cmd);
                quadIBO->bind(cmd);
                quadIBO->draw(cmd);
                RenderPipeline::endRender();
            }
        }
        void setSkyBox(VulkanCubemapImage* skyBox){
            gBufferDescriptorSet->getSamplers()[5]->setSamplerImageView(skyBox->getImageView());
            gBufferDescriptorSet->updateDescriptorSet(0);
        }
        void setGBufferPipeline(GBufferPipeline* gBufferPipeline){
            GameAssemblyPipeline::gBufferPipeline = gBufferPipeline;
            gBufferDescriptorSet->getSamplers()[0]->setSamplerImageView(gBufferPipeline->getPositionsImage()->getView());
            gBufferDescriptorSet->getSamplers()[1]->setSamplerImageView(gBufferPipeline->getAlbedoMapImage()->getView());
            gBufferDescriptorSet->getSamplers()[2]->setSamplerImageView(gBufferPipeline->getNormalMapImage()->getView());
            gBufferDescriptorSet->getSamplers()[3]->setSamplerImageView(
                    gBufferPipeline->getMetallicRoughnessEmissiveInvao()->getView());
            gBufferDescriptorSet->getSamplers()[4]->setSamplerImageView(gBufferPipeline->getAoImage()->getView());
            gBufferDescriptorSet->updateDescriptorSet(0);
        }
        void setAo(VulkanImage* ao){
            gBufferDescriptorSet->getSamplers()[4]->setSamplerImageView(ao->getView());
            gBufferDescriptorSet->updateDescriptorSet(0);
        }

        LightConfiguration &getLightConfig()  {
            return lightConfig;
        }

        VertexConfig &getVertexConfig()  {
            return vertexConfig;
        }

    };

}
