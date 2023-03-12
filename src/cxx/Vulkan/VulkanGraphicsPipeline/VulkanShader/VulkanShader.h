#pragma once

#include <vulkan/vulkan.h>
#include <vector>

class VulkanShader{
private:
    std::vector<VkShaderModule> shaderModules;
    std::vector<VkPipelineShaderStageCreateInfo> stages;
    
};