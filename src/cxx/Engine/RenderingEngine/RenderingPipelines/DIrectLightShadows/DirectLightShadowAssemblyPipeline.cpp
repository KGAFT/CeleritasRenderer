//
// Created by kgaft on 6/7/23.
//

#include "DirectLightShadowAssemblyPipeline.h"

using namespace RenderEngine;

DirectLightShadowAssemblyPipeline::DirectLightShadowAssemblyPipeline(VulkanDevice *device, int width, int height) : RenderPipeline(device, nullptr), device(device)
{
    RenderPipelineBuilder builder;
    builder.setStartFramebufferWidth(width)
        ->setStartFramebufferHeight(height)
        ->setImageRenderOutputAmount(1)
        ->setPathToShader("shaders/ShadowAssemblePipeline")
        ->addUniformBuffer(0, sizeof(ShadowAssemblerConfig), VK_SHADER_STAGE_FRAGMENT_BIT)
        ->addSampler(1, VK_SHADER_STAGE_FRAGMENT_BIT)
        ->addSampler(2, VK_SHADER_STAGE_FRAGMENT_BIT)
        ->addSampler(3, VK_SHADER_STAGE_FRAGMENT_BIT)
        ->addVertexInput(0, 3, sizeof(float), VK_FORMAT_R32G32B32_SFLOAT)
        ->addVertexInput(1, 2, sizeof(float), VK_FORMAT_R32G32_SFLOAT)
        ->addVertexInput(2, 3, sizeof(float), VK_FORMAT_R32G32B32_SFLOAT)
        ->addPushConstant(sizeof(WorldTransformData), VK_SHADER_STAGE_VERTEX_BIT);
    RenderPipeline::initialize(builder);
}

VkCommandBuffer DirectLightShadowAssemblyPipeline::beginRender()
{
    currentCmd = RenderPipeline::beginRender();
    return currentCmd;
}

void DirectLightShadowAssemblyPipeline::processMesh(Mesh *mesh)
{
    wtData.worldMatrix = mesh->getWorldMatrix();
    VulkanDescriptorSet *descriptorSet = materialDescriptors.at(mesh->getMaterial());
    descriptorSet->bind(0, currentCmd, RenderPipeline::getPipelineLayout());
    RenderPipeline::updatePushConstants();
    mesh->draw(currentCmd);
}

void DirectLightShadowAssemblyPipeline::endRender()
{
    RenderPipeline::endRenderPass();
    RenderPipeline::endRender();
}

void DirectLightShadowAssemblyPipeline::registerMaterial(Material *material)
{
    if (materialDescriptors.count(material) == 0)
    {
        VulkanDescriptorSet *descriptorSet = RenderPipeline::acquireDescriptorSet();
        descriptorSet->attachToObject(material);
        if (material->getAoTexture() != nullptr)
        {
            descriptorSet->getSamplers()[1]->setSamplerImageView(material->getAoTexture()->getView());
        }
        if (material->getNormalMap() != nullptr)
        {
            descriptorSet->getSamplers()[2]->setSamplerImageView(material->getNormalMap()->getView());
        }
        if (shadowMap != nullptr)
        {
            descriptorSet->getSamplers()[0]->setSamplerImageView(shadowMap->getView());
        }
        config.normalMapEnabled = material->getNormalMap() != nullptr;
        config.previousAoEnabled = material->getAoTexture() != nullptr;
        descriptorSet->getUniformBuffers()[0]->write(&config);
        descriptorSet->updateDescriptorSet(0);
        materialDescriptors.insert(std::pair(material, descriptorSet));
    }
}

void DirectLightShadowAssemblyPipeline::setShadowMap(VulkanImage *shadowMap)
{
    this->shadowMap = shadowMap;
    for (auto &item : materialDescriptors)
    {
        item.second->getSamplers()[0]->setSamplerImageView(shadowMap->getView());
        item.second->updateDescriptorSet(0);
    }
}

void DirectLightShadowAssemblyPipeline::unRegisterMeshMaterial(Material *material)
{
    if (materialDescriptors.count(material) > 0)
    {
        materialDescriptors.at(material)->attachToObject(nullptr);
        materialDescriptors.erase(material);
    }
}

ShadowAssemblerConfig &DirectLightShadowAssemblyPipeline::getConfig()
{
    return config;
}

WorldTransformData &DirectLightShadowAssemblyPipeline::getWorldTransformData()
{
    return wtData;
}
