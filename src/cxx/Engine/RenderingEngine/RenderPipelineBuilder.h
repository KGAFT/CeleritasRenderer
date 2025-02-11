//
// Created by kgaft on 6/6/23.
//
#pragma once

#include <Vulkan/VulkanGraphicsPipeline/PipelineEndConfiguration.h>

namespace RenderEngine{
    class RenderPipelineBuilder {
        friend class RenderPipeline;
    public:
        RenderPipelineBuilder();
    private:
        PipelineEndConfig endConfiguration{};
        const char* pathToShader = nullptr;
        unsigned int imageRenderOutputAmount = 0;
        int startFramebufferWidth = 0;
        int startFramebufferHeight = 0;
        bool alphaBlending = true;
        VkCullModeFlags culling = VK_CULL_MODE_NONE;
    public:
        RenderPipelineBuilder* addPushConstant(size_t structSize, VkShaderStageFlags shaderStages);

        RenderPipelineBuilder* addUniformBuffer(int binding, size_t size, VkShaderStageFlags shaderStages);

        RenderPipelineBuilder* addVertexInput(unsigned int location, unsigned int coordinatesAmount, size_t typeSize, VkFormat inputFormat);

        RenderPipelineBuilder* addSampler(int binding, VkShaderStageFlags shaderStages);

        RenderPipelineBuilder* setPathToShader(const char* path);

        RenderPipelineBuilder* setImageRenderOutputAmount(unsigned int amount);

        RenderPipelineBuilder* setStartFramebufferWidth(int startFramebufferWidth);

        RenderPipelineBuilder* setStartFramebufferHeight(int startFramebufferHeight);

        RenderPipelineBuilder* setAlphablending(bool alphaBlending);

        RenderPipelineBuilder* setCulling(VkCullModeFlags culling);
    private:
        bool isComplete();
    };
}




