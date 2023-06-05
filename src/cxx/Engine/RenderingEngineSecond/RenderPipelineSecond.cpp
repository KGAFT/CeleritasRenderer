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

