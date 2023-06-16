//
// Created by kgaft on 6/6/23.
//

#include "RenderPipelineBuilder.h"

RenderEngine::RenderPipelineBuilder* RenderEngine::RenderPipelineBuilder::addPushConstant(size_t structSize, VkShaderStageFlags shaderStages) {
    endConfiguration.pushConstantInfos.push_back({shaderStages, structSize});
    return this;
}

RenderEngine::RenderPipelineBuilder*  RenderEngine::RenderPipelineBuilder::addUniformBuffer(int binding, size_t size, VkShaderStageFlags shaderStages) {
    endConfiguration.uniformBuffers.push_back({binding, size, shaderStages});
    return this;
}

RenderEngine::RenderPipelineBuilder* RenderEngine::RenderPipelineBuilder::addVertexInput(unsigned int location, unsigned int coordinatesAmount, size_t typeSize,
                                           VkFormat inputFormat) {
    endConfiguration.vertexInputs.push_back({location, coordinatesAmount, typeSize, inputFormat});
    return this;
}

RenderEngine::RenderPipelineBuilder* RenderEngine::RenderPipelineBuilder::addSampler(int binding, VkShaderStageFlags shaderStages) {
    endConfiguration.samplers.push_back({binding, shaderStages});
    return this;
}

RenderEngine::RenderPipelineBuilder* RenderEngine::RenderPipelineBuilder::setPathToShader(const char *path) {
    this->pathToShader = path;
    return this;
}

RenderEngine::RenderPipelineBuilder* RenderEngine::RenderPipelineBuilder::setImageRenderOutputAmount(unsigned int amount) {
    this->imageRenderOutputAmount = amount;
    return this;
}

bool RenderEngine::RenderPipelineBuilder::isComplete() {
    return !endConfiguration.vertexInputs.empty() and pathToShader!=nullptr and startFramebufferHeight!=0 and startFramebufferWidth!=0 and imageRenderOutputAmount!=0;
}

RenderEngine::RenderPipelineBuilder* RenderEngine::RenderPipelineBuilder::setStartFramebufferWidth(int startFramebufferWidth) {
    this->startFramebufferWidth = startFramebufferWidth;
    return this;
}

RenderEngine::RenderPipelineBuilder* RenderEngine::RenderPipelineBuilder::setStartFramebufferHeight(int startFramebufferHeight) {
    this->startFramebufferHeight = startFramebufferHeight;
    return this;
}

RenderEngine::RenderPipelineBuilder* RenderEngine::RenderPipelineBuilder::setAlphablending(bool alphaBlending)
{
    this->alphaBlending = alphaBlending;
    return this;
}

RenderEngine::RenderPipelineBuilder* RenderEngine::RenderPipelineBuilder::setCulling(VkCullModeFlags culling)
{
    this->culling = culling;
    return this;
}

RenderEngine::RenderPipelineBuilder::RenderPipelineBuilder() {

}

