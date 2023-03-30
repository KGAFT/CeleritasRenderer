//
// Created by KGAFT on 3/20/2023.
//
#pragma once

#include "../../../Vulkan/VulkanDevice/VulkanDevice.h"
#include "../../../Vulkan/VulkanEndRenderPipeline.h"
#include "../../../Vulkan/VulkanImage/VulkanImage.h"
#include "../../../Util/ShaderLoader.h"
#include "../../GraphicalObjects/Mesh.h"
#include <glm/glm.hpp>



struct GBufferConfig {
    alignas(4) int combinedMetallicRoughness = 0;
    alignas(4) int opacityMapEnabled = 0;
    alignas(4) int emissiveEnabled = 0;
    alignas(4) int aoEnabled = 0;
};
struct PushConstantWorldViewData {
    glm::mat4 viewMatrix;
    glm::mat4 worldMatrix;
    glm::vec3 cameraPosition;
};

class GBufferPipeline {
private:
    VulkanDevice *device;
    VulkanSyncManager* syncManager;
    VulkanEndRenderPipeline *endRenderPipeline;
    VulkanShader *shader;
    VulkanImage *vertices;
    VulkanImage *albedo;
    VulkanImage *normal;
    VulkanImage *metallicRoughnessEmissiveINVaO;
    VulkanUniformBuffer *configUbo;
    VulkanPushConstant *worldViewData;

    VulkanSampler *albedoMapSampler;
    VulkanSampler *normalMapSampler;
    VulkanSampler *metallicMapSampler;
    VulkanSampler *roughnessMapSampler;
    VulkanSampler *aoMapSampler;
    VulkanSampler *emissiveMapSampler;
    VulkanSampler *metallicRoughnessSampler;
    VulkanSampler *opacityMapSampler;

    float width;
    float height;
    float scale = 1;

    PushConstantWorldViewData viewData;
    GBufferConfig config;
public:
    GBufferPipeline(VulkanDevice *device, int startWidth, int startHeight) : device(device) {
        this->width = startWidth*this->scale;
        this->height = startHeight*this->scale;
        shader = ShaderLoader::loadShaders("shaders/GBufferPBR", device);
        syncManager = new VulkanSyncManager(device, nullptr);


        vertices = VulkanImage::createImage(device, width, height);
        albedo = VulkanImage::createImage(device, width, height);
        normal = VulkanImage::createImage(device, width, height);
        metallicRoughnessEmissiveINVaO = VulkanImage::createImage(device, width, height);
        loadPipeline();
    }
    VkCommandBuffer beginRender() {
        endRenderPipeline->getPushConstants()[0]->setData(&viewData);
        endRenderPipeline->getUniformBuffers()[0]->write(&config);
        return endRenderPipeline->beginRender();
    }

    void updatePcs(){
        endRenderPipeline->getPushConstants()[0]->setData(&viewData);
        endRenderPipeline->updatePcs();
    }
    void updateUniforms(){
        endRenderPipeline->getUniformBuffers()[0]->write(&config);
        endRenderPipeline->updateUniforms();
    }
    void populateSamplers(Material* material){
        config.aoEnabled = material->getAoTexture()!=nullptr;
        config.combinedMetallicRoughness = material->getMetallicRoughnessTexture()!=nullptr;
        config.emissiveEnabled = material->getEmissiveTexture()!=nullptr;
        config.opacityMapEnabled = material->getOpacityMapTexture()!=nullptr;
        if(material->getAlbedoTexture()!=nullptr){
            albedoMapSampler->setSamplerImageView(material->getAlbedoTexture()->getView());
        }
        normalMapSampler->setSamplerImageView(material->getNormalMap()->getView());
        if(material->getMetallicRoughnessTexture()!=nullptr){
            metallicRoughnessSampler->setSamplerImageView(material->getMetallicRoughnessTexture()->getView());
        }
        else{
            metallicMapSampler->setSamplerImageView(material->getMetallicTexture()->getView());
            roughnessMapSampler->setSamplerImageView(material->getRoughnessTexture()->getView());
        }
        if(config.aoEnabled){
            aoMapSampler->setSamplerImageView(material->getAoTexture()->getView());
        }
        else if(aoMapSampler->getImageView()==VK_NULL_HANDLE){
            aoMapSampler->setSamplerImageView(material->getAlbedoTexture()->getView());
        }
        if(config.emissiveEnabled){
            emissiveMapSampler->setSamplerImageView(material->getEmissiveTexture()->getView());
        }
        else if(emissiveMapSampler->getImageView()==VK_NULL_HANDLE){
            emissiveMapSampler->setSamplerImageView(material->getAlbedoTexture()->getView());
        }
        if(config.opacityMapEnabled){
            opacityMapSampler->setSamplerImageView(material->getOpacityMapTexture()->getView());
        }
        else if(opacityMapSampler->getImageView()==VK_NULL_HANDLE){
            opacityMapSampler->setSamplerImageView(material->getAlbedoTexture()->getView());
        }
    }
    void endRender() {
        endRenderPipeline->endRender();
    }

    VulkanImage *getVertices()  {
        return vertices;
    }

    VulkanImage *getAlbedo()  {
        return albedo;
    }

    VulkanImage *getNormal()  {
        return normal;
    }

    VulkanImage *getMetallicRoughnessEmissiveInVaO()  {
        return metallicRoughnessEmissiveINVaO;
    }

    VulkanSampler *getAlbedoMapSampler()  {
        return albedoMapSampler;
    }

    VulkanSampler *getNormalMapSampler()  {
        return normalMapSampler;
    }

    VulkanSampler *getMetallicMapSampler()  {
        return metallicMapSampler;
    }

    VulkanSampler *getRoughnessMapSampler()  {
        return roughnessMapSampler;
    }

    VulkanSampler *getAoMapSampler()  {
        return aoMapSampler;
    }

    VulkanSampler *getEmissiveMapSampler()  {
        return emissiveMapSampler;
    }

    VulkanSampler *getMetallicRoughnessSampler()  {
        return metallicRoughnessSampler;
    }

    VulkanSampler *getOpacityMapSampler()  {
        return opacityMapSampler;
    }

    PushConstantWorldViewData &getViewData() {
        return viewData;
    }

    GBufferConfig &getConfig() {
        return config;
    }

    float getScale() {
        return scale;
    }

    void setScale(float scale) {
        GBufferPipeline::scale = scale;
    }
    void newSize(float width, float height) {
        this->width = width;
        this->height = height;
    }
    void resized() {
        vkDeviceWaitIdle(device->getDevice());
        delete vertices;
        delete albedo;
        delete normal;
        delete metallicRoughnessEmissiveINVaO;
        vertices = VulkanImage::createImage(device, width * scale, height * scale);
        albedo = VulkanImage::createImage(device, width * scale, height * scale);
        normal = VulkanImage::createImage(device, width * scale, height * scale);
        metallicRoughnessEmissiveINVaO = VulkanImage::createImage(device, width * scale, height * scale);
        std::vector<VkImageView> imagesToRender;
        imagesToRender.push_back(vertices->getView());
        imagesToRender.push_back(albedo->getView());
        imagesToRender.push_back(normal->getView());
        imagesToRender.push_back(metallicRoughnessEmissiveINVaO->getView());

        endRenderPipeline->resized(width * scale, height * scale, &imagesToRender, 4, albedo->getFormat());
    }
private:
    void loadPipeline(){
        PipelineEndConfig endConfig{};
        endConfig.vertexInputs.push_back({0, 3, sizeof(float), VK_FORMAT_R32G32B32_SFLOAT});
        endConfig.vertexInputs.push_back({1, 2, sizeof(float), VK_FORMAT_R32G32_SFLOAT});
        endConfig.vertexInputs.push_back({2, 3, sizeof(float), VK_FORMAT_R32G32B32_SFLOAT});
        endConfig.pushConstantInfos.push_back({VK_SHADER_STAGE_VERTEX_BIT, sizeof(PushConstantWorldViewData)});
        endConfig.uniformBuffers.push_back({0, sizeof(GBufferConfig), VK_SHADER_STAGE_FRAGMENT_BIT});

        endConfig.samplers.push_back({1, VK_SHADER_STAGE_FRAGMENT_BIT});
        endConfig.samplers.push_back({2, VK_SHADER_STAGE_FRAGMENT_BIT});
        endConfig.samplers.push_back({3, VK_SHADER_STAGE_FRAGMENT_BIT});
        endConfig.samplers.push_back({4, VK_SHADER_STAGE_FRAGMENT_BIT});
        endConfig.samplers.push_back({5, VK_SHADER_STAGE_FRAGMENT_BIT});
        endConfig.samplers.push_back({6, VK_SHADER_STAGE_FRAGMENT_BIT});
        endConfig.samplers.push_back({7, VK_SHADER_STAGE_FRAGMENT_BIT});
        endConfig.samplers.push_back({8, VK_SHADER_STAGE_FRAGMENT_BIT});

        std::vector<VkImageView> imagesToRender;
        imagesToRender.push_back(vertices->getView());
        imagesToRender.push_back(albedo->getView());
        imagesToRender.push_back(normal->getView());
        imagesToRender.push_back(metallicRoughnessEmissiveINVaO->getView());
        endRenderPipeline = new VulkanEndRenderPipeline(device, syncManager, shader, &endConfig, width, height, imagesToRender, 4, albedo->getFormat());
        albedoMapSampler = endRenderPipeline->getSamplers()[0];
        normalMapSampler = endRenderPipeline->getSamplers()[1];
        metallicMapSampler = endRenderPipeline->getSamplers()[2];
        roughnessMapSampler = endRenderPipeline->getSamplers()[3];
        aoMapSampler = endRenderPipeline->getSamplers()[4];
        emissiveMapSampler = endRenderPipeline->getSamplers()[5];
        metallicRoughnessSampler = endRenderPipeline->getSamplers()[6];
        opacityMapSampler = endRenderPipeline->getSamplers()[7];
        configUbo = endRenderPipeline->getUniformBuffers()[0];
        worldViewData = endRenderPipeline->getPushConstants()[0];
    }
};