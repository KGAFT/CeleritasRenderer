#pragma once

#include <vulkan/vulkan.h>

struct VertexInput{
    unsigned int location;
    unsigned int coordinatesAmount;
    size_t typeSize;
};

struct PushConstantInfo{
    VkShaderStageFlags shaderStages;
    size_t size;
};

struct UniformBufferInfo{
    int location;
    size_t size;
    VkShaderStageFlags shaderStages;
};

struct SamplerInfo{
    int location;
    VkShaderStageFags shaderStages;
};

struct PipelineEndConfig{
    std::vector<SamplerInfo> samplers;
    std::vector<UniformBufferInfo> uniformBuffers;
    std::vector<VertexInput> vertexInputs;
};