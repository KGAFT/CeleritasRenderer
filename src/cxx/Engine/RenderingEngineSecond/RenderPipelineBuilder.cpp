//
// Created by kgaft on 6/6/23.
//

#include "RenderPipelineBuilder.h"

void RenderEngine::RenderPipelineBuilder::addPushConstant(size_t structSize, VkShaderStageFlags shaderStages) {
    endConfiguration.pushConstantInfos.push_back({shaderStages, structSize});
}

void RenderEngine::RenderPipelineBuilder::addUniformBuffer(int binding, size_t size, VkShaderStageFlags shaderStages) {
    endConfiguration.uniformBuffers.push_back({binding, size, shaderStages});
}

void RenderEngine::RenderPipelineBuilder::addVertexInput(unsigned int location, unsigned int coordinatesAmount, size_t typeSize,
                                           VkFormat inputFormat) {
    endConfiguration.vertexInputs.push_back({location, coordinatesAmount, typeSize, inputFormat});
}

void RenderEngine::RenderPipelineBuilder::addSampler(int binding, VkShaderStageFlags shaderStages) {
    endConfiguration.samplers.push_back({binding, shaderStages});
}

void RenderEngine::RenderPipelineBuilder::setPathToShader(const char *path) {
    this->pathToShader = path;
}

void RenderEngine::RenderPipelineBuilder::setImageRenderOutputAmount(unsigned int amount) {
    this->imageRenderOutputAmount = amount;
}

bool RenderEngine::RenderPipelineBuilder::isComplete() {
    return !endConfiguration.vertexInputs.empty() and pathToShader!=nullptr and startFramebufferHeight!=0 and startFramebufferWidth!=0;
}

void RenderEngine::RenderPipelineBuilder::setStartFramebufferWidth(int startFramebufferWidth) {
    this->startFramebufferWidth = startFramebufferWidth;
}

void RenderEngine::RenderPipelineBuilder::setStartFramebufferHeight(int startFramebufferHeight) {
    this->startFramebufferHeight = startFramebufferHeight;
}

