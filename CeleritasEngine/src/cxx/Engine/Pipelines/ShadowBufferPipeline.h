//
// Created by Daniil on 14.04.2023.
//
#pragma once

#include "../../Vulkan/VulkanDevice/VulkanDevice.h"
#include "../../Vulkan/VulkanGraphicsPipeline/VulkanShader/VulkanShader.h"

class ShadowBufferPipeline{
private:
    VulkanDevice* device;
    VulkanShader* shader;
};