//
// Created by kgaft on 6/6/23.
//

#include "RenderPipelineSecond.h"

RenderEngine::RenderPipelineSecond::RenderPipelineSecond(VulkanDevice *device, VulkanSwapChain swapChain) {

}

void RenderEngine::RenderPipelineSecond::initialize(RenderEngine::RenderPipelineBuilder &builder) {
    if(builder.isComplete()){
        syncManager = new VulkanSyncManager(device, swapChain);
        shader = ShaderLoader::loadShaders(builder.pathToShader, device);

    }
    else{
        throw std::runtime_error("Error you does not complete the Pipeline builder");
    }

    if(!initializeRenderOutputs(builder)){
        endRenderPipeline = new VulkanEndRenderPipeline(device, syncManager, shader,
                                                        &builder.endConfiguration,
                                                        builder.startFramebufferWidth,
                                                        builder.startFramebufferHeight, swapChain->getSwapChainImageViews(),
                                                        0, swapChain->getSwapChainImageFormat());
    }
    else{
        endRenderPipeline = new VulkanEndRenderPipeline(device, syncManager, shader,
                                                        &builder.endConfiguration,
                                                        builder.startFramebufferWidth,
                                                        builder.startFramebufferHeight,
                                                        imageViews,
                                                        builder.imageRenderOutputAmount,
                                                        outputImages[0]->getFormat());
    }
    this->imagePerStepAmount = builder.imageRenderOutputAmount;
}

bool RenderEngine::RenderPipelineSecond::initializeRenderOutputs(RenderEngine::RenderPipelineBuilder &builder) {
    if(builder.imageRenderOutputAmount==0 and swapChain==nullptr){
        throw std::runtime_error("Error: you did not specified output images");
    }
    if(swapChain!=nullptr){
        return false;
    }
    for (int i = 0; i < builder.imageRenderOutputAmount; ++i){
        outputImages.push_back(VulkanImage::createImage(device, builder.startFramebufferWidth, builder.startFramebufferHeight));
        imageViews.push_back(outputImages[i]->getView());
    }
    return true;
}

VulkanDescriptorSet* RenderEngine::RenderPipelineSecond::acquireDescriptorSet() {
    return endRenderPipeline->acquireDescriptorSet();
}

void RenderEngine::RenderPipelineSecond::releaseDescriptorSet(VulkanDescriptorSet *descriptorSet) {
    descriptorSet->attachToObject(nullptr);
}

std::vector<VulkanPushConstant *> &RenderEngine::RenderPipelineSecond::getPushConstants() {
    return endRenderPipeline->getPushConstants();
}

VkCommandBuffer RenderEngine::RenderPipelineSecond::beginRender() {
    VkCommandBuffer cmd = endRenderPipeline->beginRender();
    if (endRenderPipeline->getPushConstants().size() > 0)
    {
        endRenderPipeline->updatePushConstants();
    }
    return cmd;
}

void RenderEngine::RenderPipelineSecond::endRenderPass() {
    endRenderPipeline->endRenderPass();
}

void RenderEngine::RenderPipelineSecond::endRender() {
    endRenderPipeline->endRender();
}

std::vector<VkImage>& RenderEngine::RenderPipelineSecond::getDepthImages(){
    return endRenderPipeline->getDepthImages();
}

std::vector<VulkanImage *> &RenderEngine::RenderPipelineSecond::getOutputImages()  {
    return outputImages;
}

void RenderEngine::RenderPipelineSecond::resize(int width, int height) {
    vkDeviceWaitIdle(device->getDevice());
    if(swapChain==nullptr){
        imageViews.clear();
        for (const auto &item: outputImages){
            item->resize(width, height);
            imageViews.push_back(item->getView());
        }
        endRenderPipeline->resized(width, height, imageViews, this->imagePerStepAmount, outputImages[0]->getFormat());
    }
    else{
        endRenderPipeline->resized(width,height);
    }
}



