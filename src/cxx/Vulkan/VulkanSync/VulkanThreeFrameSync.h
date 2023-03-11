//
// Created by KGAFT on 3/11/2023.
//
#pragma once

#include <vulkan/vulkan.h>

#include <vector>
class VulkanThreeFrameSync
{
private:
    std::vector<int> integers;
    VkImageCreateInfo createInfo{};
};