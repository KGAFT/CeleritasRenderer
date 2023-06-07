//
// Created by kgaft on 6/7/23.
//

#include "AssemblyPipeline.h"

RenderEngine::AssemblyPipeline::AssemblyPipeline(VulkanDevice *device, VulkanSwapChain *swapChain, int width, int height) : RenderPipelineSecond(device, swapChain), device(device), swapChain(swapChain){
    RenderPipelineBuilder builder;
    builder.setImageRenderOutputAmount(1)
    ->addPushConstant(sizeof(AssemblyPipelineSetup), VK_SHADER_STAGE_FRAGMENT_BIT)
            ->addSampler(1, VK_SHADER_STAGE_FRAGMENT_BIT)
            ->addVertexInput(0, 3, sizeof(float), VK_FORMAT_R32G32B32_SFLOAT)
            ->addVertexInput(1, 2, sizeof(float), VK_FORMAT_R32G32_SFLOAT)
            ->setStartFramebufferWidth(width)->setStartFramebufferHeight(height)->setPathToShader("shaders/OutputPipeline");
    RenderPipelineSecond::initialize(builder);
    inputsDescriptorSet = RenderPipelineSecond::acquireDescriptorSet();
    inputsDescriptorSet->attachToObject(this);

    quadDrawMesh = acquireQuadMesh(device);
    setup = {};
    RenderPipelineSecond::getPushConstants()[0]->setData(&setup);
}

void RenderEngine::AssemblyPipeline::setGamePlaceHolder(VulkanImage* placeHolder) {
    inputsDescriptorSet->getSamplers()[0]->setSamplerImageView(placeHolder->getView());
    for(int i = 0; i<3; i++){
        inputsDescriptorSet->updateDescriptorSet(i);
    }
}

void RenderEngine::AssemblyPipeline::update() {
    VkCommandBuffer cmd = RenderPipelineSecond::beginRender();
    quadDrawMesh->draw(cmd);
    RenderPipelineSecond::endRenderPass();
    RenderPipelineSecond::endRender();
}

