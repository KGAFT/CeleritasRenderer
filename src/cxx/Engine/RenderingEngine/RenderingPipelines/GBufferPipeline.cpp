//
// Created by kgaft on 6/7/23.
//

#include "GBufferPipeline.h"

RenderEngine::GBufferPipeline::GBufferPipeline(VulkanDevice *device, int width, int height) : RenderPipeline(device, nullptr){
    RenderPipelineBuilder builder;
    builder.setPathToShader("shaders/GBufferPBR")
    ->addVertexInput(0, 3, sizeof(float), VK_FORMAT_R32G32B32_SFLOAT)
    ->addVertexInput(1, 2, sizeof(float), VK_FORMAT_R32G32_SFLOAT)
    ->addVertexInput(2, 3, sizeof(float), VK_FORMAT_R32G32B32_SFLOAT)
            ->addPushConstant(sizeof(WorldTransformData), VK_SHADER_STAGE_VERTEX_BIT)
            ->addUniformBuffer(0, sizeof(GBufferConfig), VK_SHADER_STAGE_FRAGMENT_BIT);
    for(int i = 1; i<9; i++){
        builder.addSampler(i, VK_SHADER_STAGE_FRAGMENT_BIT);
    }
    builder.setStartFramebufferWidth(width)->setStartFramebufferHeight(height)->setImageRenderOutputAmount(5);
    RenderPipeline::initialize(builder);
    RenderPipeline::getPushConstants()[0]->setData(&worldTransformData);
}

void RenderEngine::GBufferPipeline::registerMaterial(Material *material) {
    VulkanDescriptorSet* descriptorSet;
    if(materialDescriptors.count(material)==0){
        descriptorSet = RenderPipeline::acquireDescriptorSet();
        descriptorSet->attachToObject(material);
    }
    else{
        descriptorSet = materialDescriptors.at(material);
    }
    gBufferConfig.aoEnabled = material->getAoTexture()!=nullptr;
    gBufferConfig.combinedMetallicRoughness = material->getMetallicRoughnessTexture()!=nullptr;
    gBufferConfig.emissiveEnabled = material->getEmissiveTexture()!=nullptr;
    gBufferConfig.opacityMapEnabled = material->getOpacityMapTexture()!=nullptr;
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
    currentCmd = RenderPipeline::beginRender();
}

void RenderEngine::GBufferPipeline::drawMesh(Mesh *mesh) {
    if(materialDescriptors.count(mesh->getMaterial())==0){
        throw std::runtime_error("Error: your mesh is use unregistered material, please register it!");
    }
    VulkanDescriptorSet* materialDescriptor = materialDescriptors.at(mesh->getMaterial());

    materialDescriptor->bind(0, currentCmd, RenderPipeline::getPipelineLayout());
    worldTransformData.worldMatrix = mesh->getWorldMatrix();
    RenderPipeline::updatePushConstants();
    mesh->draw(currentCmd);

}

void RenderEngine::GBufferPipeline::endRender() {
    RenderPipeline::endRenderPass();
    RenderPipeline::endRender();
}

RenderEngine::WorldTransformData * RenderEngine::GBufferPipeline::getWorldTransformData()
{
    return &worldTransformData;
}

VulkanImage *RenderEngine::GBufferPipeline::getPositionsImage() {
    return RenderPipeline::getOutputImages()[0];
}

VulkanImage *RenderEngine::GBufferPipeline::getAlbedoMapImage() {
    return RenderPipeline::getOutputImages()[1];
}

VulkanImage *RenderEngine::GBufferPipeline::getNormalMapImage() {
    return RenderPipeline::getOutputImages()[2];
}

VulkanImage *RenderEngine::GBufferPipeline::getMetallicRoughnessEmissiveInvao()  {
    return RenderPipeline::getOutputImages()[3];
}

VulkanImage* RenderEngine::GBufferPipeline::getAoImage(){
    return RenderPipeline::getOutputImages()[4];
}