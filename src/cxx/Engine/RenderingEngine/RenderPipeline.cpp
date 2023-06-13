//
// Created by kgaft on 6/6/23.
//

#include "RenderPipeline.h"

RenderEngine::RenderPipeline::RenderPipeline(VulkanDevice *device, VulkanSwapChain* swapChain) : device(device), swapChain(swapChain) {

}

void RenderEngine::RenderPipeline::initialize(RenderEngine::RenderPipelineBuilder &builder) {
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
                                                        builder.imageRenderOutputAmount, swapChain->getSwapChainImageFormat());
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

bool RenderEngine::RenderPipeline::initializeRenderOutputs(RenderEngine::RenderPipelineBuilder &builder) {
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

VulkanDescriptorSet* RenderEngine::RenderPipeline::acquireDescriptorSet() {
    return endRenderPipeline->acquireDescriptorSet();
}

void RenderEngine::RenderPipeline::releaseDescriptorSet(VulkanDescriptorSet *descriptorSet) {
    descriptorSet->attachToObject(nullptr);
}

std::vector<VulkanPushConstant *> &RenderEngine::RenderPipeline::getPushConstants() {
    return endRenderPipeline->getPushConstants();
}

VkCommandBuffer RenderEngine::RenderPipeline::beginRender() {
    VkCommandBuffer cmd = endRenderPipeline->beginRender();
    if (endRenderPipeline->getPushConstants().size() > 0)
    {
        endRenderPipeline->updatePushConstants();
    }
    return cmd;
}

void RenderEngine::RenderPipeline::endRenderPass() {
    endRenderPipeline->endRenderPass();
}

void RenderEngine::RenderPipeline::endRender() {
    endRenderPipeline->endRender();
}

std::vector<VkImage>& RenderEngine::RenderPipeline::getDepthImages(){
    return endRenderPipeline->getDepthImages();
}

std::vector<VulkanImage *> &RenderEngine::RenderPipeline::getOutputImages()  {
    return outputImages;
}

void RenderEngine::RenderPipeline::resize(int width, int height) {
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

void RenderEngine::RenderPipeline::initUi(GLFWwindow *window){
    endRenderPipeline->initIMGUI(window);
}

VkPipelineLayout RenderEngine::RenderPipeline::getPipelineLayout() {
    return endRenderPipeline->getPipelineLayout();
}

void RenderEngine::RenderPipeline::updatePushConstants() {
    endRenderPipeline->updatePushConstants();
}



