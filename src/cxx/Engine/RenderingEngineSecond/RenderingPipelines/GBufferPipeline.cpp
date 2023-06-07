//
// Created by kgaft on 6/7/23.
//

#include "GBufferPipeline.h"

RenderEngine::GBufferPipeline::GBufferPipeline(VulkanDevice *device, int width, int height) : RenderPipelineSecond(device, nullptr){
    RenderPipelineBuilder builder;
    builder.setPathToShader("shader/GBufferPipeline")
    ->addVertexInput(0, 3, sizeof(float), VK_FORMAT_R32G32B32_SFLOAT)
    ->addVertexInput(0, 2, sizeof(float), VK_FORMAT_R32G32_SFLOAT)
    ->addVertexInput(0, 3, sizeof(float), VK_FORMAT_R32G32B32_SFLOAT)
            ->addPushConstant(sizeof(WorldTransformData), VK_SHADER_STAGE_VERTEX_BIT)
            ->addUniformBuffer(0, sizeof(GBufferConfig), VK_SHADER_STAGE_FRAGMENT_BIT);
    for(int i = 1; i<9; i++){
        builder.addSampler(i, VK_SHADER_STAGE_FRAGMENT_BIT);
    }
    builder.setStartFramebufferWidth(width)->setStartFramebufferHeight(height);
    RenderPipelineSecond::initialize(builder);
    RenderPipelineSecond::getPushConstants()[0]->setData(&worldTransformData);
}

void RenderEngine::GBufferPipeline::registerMaterial(Material *material) {
    VulkanDescriptorSet* descriptorSet;
    if(materialDescriptors.count(material)==0){
        descriptorSet = RenderPipelineSecond::acquireDescriptorSet();
        descriptorSet->attachToObject(material);
    }
    else{
        descriptorSet = materialDescriptors.at(material);
    }
    descriptorSet->getUniformBuffers()[0]->write(&gBufferConfig);

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
    descriptorSet->updateDescriptorSet(0);
    materialDescriptors.insert(std::pair<Material*, VulkanDescriptorSet*>(material, descriptorSet));
}

void RenderEngine::GBufferPipeline::unRegisterMaterial(Material *material) {
    if(materialDescriptors.count(material)>0){
        materialDescriptors.at(material)->attachToObject(nullptr);
        materialDescriptors.erase(material);
    }
}

void RenderEngine::GBufferPipeline::beginRender() {
    currentCmd = RenderPipelineSecond::beginRender();
}

void RenderEngine::GBufferPipeline::drawMesh(Mesh *mesh) {
    if(materialDescriptors.count(mesh->getMaterial())==0){
        throw std::runtime_error("Error: your mesh is use unregistered material, please register it!");
    }
    VulkanDescriptorSet* materialDescriptor = materialDescriptors.at(mesh->getMaterial());
    materialDescriptor->getUniformBuffers()[0]->write(&gBufferConfig);
    materialDescriptor->bind(0, currentCmd, RenderPipelineSecond::getPipelineLayout());
    worldTransformData.worldMatrix = mesh->getWorldMatrix();
    RenderPipelineSecond::updatePushConstants();
    mesh->draw(currentCmd);

}

void RenderEngine::GBufferPipeline::endRender() {
    RenderPipelineSecond::endRenderPass();
    RenderPipelineSecond::endRender();
}

VulkanImage *RenderEngine::GBufferPipeline::getPositionsImage() {
    return RenderPipelineSecond::getOutputImages()[0];
}

VulkanImage *RenderEngine::GBufferPipeline::getAlbedoMapImage() {
    return RenderPipelineSecond::getOutputImages()[1];
}

VulkanImage *RenderEngine::GBufferPipeline::getNormalMapImage() {
    return RenderPipelineSecond::getOutputImages()[2];
}

VulkanImage *RenderEngine::GBufferPipeline::getMetallicRoughnessEmissiveInvao()  {
    return RenderPipelineSecond::getOutputImages()[3];
}

VulkanImage* RenderEngine::GBufferPipeline::getAoImage(){
    return RenderPipelineSecond::getOutputImages()[4];
}