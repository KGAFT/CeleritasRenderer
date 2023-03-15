//
// Created by KGAFT on 3/15/2023.
//
#pragma once
#include <vulkan/vulkan.h>
class IDescriptorObject{
    virtual VkDescriptorType getDescriptorType() = 0;
    /**
     * @return nullptr_t if not sampler
     */
    virtual VkImageView getImageView() = 0;
    /**
     * @return nullptr_t if not sampler
     */
    virtual VkSampler getSampler() = 0;

    /**
     * @return nullptr_t if not ubo;
     */
    virtual VkBuffer getBuffer() = 0;
    /**
    * @return nullptr_t if not ubo;
    */
    virtual size_t getBufferSize() = 0;
};