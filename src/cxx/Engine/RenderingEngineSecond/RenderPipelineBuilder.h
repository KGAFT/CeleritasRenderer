//
// Created by kgaft on 6/6/23.
//
#pragma once

#include <Vulkan/VulkanGraphicsPipeline/PipelineEndConfiguration.h>

namespace RenderEngine{
    class RenderPipelineBuilder {
        friend class RenderPipelineSecond;
    public:
        RenderPipelineBuilder();
    private:
        PipelineEndConfig endConfiguration;
        const char* pathToShader = nullptr;
        unsigned int imageRenderOutputAmount = 0;
        int startFramebufferWidth = 0;
        int startFramebufferHeight = 0;

    public:
        void addPushConstant(size_t structSize, VkShaderStageFlags shaderStages);

        void addUniformBuffer(int binding, size_t size, VkShaderStageFlags shaderStages);

        void addVertexInput(unsigned int location, unsigned int coordinatesAmount, size_t typeSize, VkFormat inputFormat);

        void addSampler(int binding, VkShaderStageFlags shaderStages);

        void setPathToShader(const char* path);

        void setImageRenderOutputAmount(unsigned int amount);

        void setStartFramebufferWidth(int startFramebufferWidth);

        void setStartFramebufferHeight(int startFramebufferHeight);

    private:
        bool isComplete();
    };
}




