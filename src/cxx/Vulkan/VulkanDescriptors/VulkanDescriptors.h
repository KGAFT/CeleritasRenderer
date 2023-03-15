//
// Created by KGAFT on 3/15/2023.
//

#pragma once

#include <vulkan/vulkan.h>
#include "../VulkanDevice/VulkanDevice.h"

class VulkanDescriptors{
private:
    VkDescriptorPool descriptorPool;
    VulkanDevice *device;
    std::vector<VkDescriptorSet> descriptorSets;
public:
    VulkanDescriptors(VulkanDevice *device, VkDescriptorSetLayout layout, unsigned int instanceCount) {
        this->device = device;
        std::vector<VkDescriptorPoolSize> sizes;
        for (int i = 0; i < 100; ++i) {
            VkDescriptorPoolSize poolSize{};
            if (i % 2 == 0) {
                poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            } else {
                poolSize.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            }
            poolSize.descriptorCount = instanceCount;
            sizes.push_back(poolSize);
        }
        VkDescriptorPoolCreateInfo poolInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        poolInfo.poolSizeCount = sizes.size();
        poolInfo.pPoolSizes = sizes.data();
        poolInfo.maxSets = instanceCount;

        if (vkCreateDescriptorPool(device->getDevice(), &poolInfo, nullptr, &descriptorPool) != VK_SUCCESS) {
            throw std::runtime_error("failed to create descriptor pool!");
        }
        createDescriptorSets(layout, instanceCount);
    }
private:
    void createDescriptorSets(VkDescriptorSetLayout layout, unsigned int instanceCount) {
        std::vector<VkDescriptorSetLayout> layouts(instanceCount, layout);
        VkDescriptorSetAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfo.descriptorPool = descriptorPool;
        allocInfo.descriptorSetCount = instanceCount;
        allocInfo.pSetLayouts = layouts.data();
        descriptorSets.resize(instanceCount);
        VkResult result = vkAllocateDescriptorSets(device->getDevice(), &allocInfo, descriptorSets.data());
        if (result != VK_SUCCESS) {
            switch (result) {
                case VK_ERROR_OUT_OF_HOST_MEMORY:
                    std::cerr << "OHM" << std::endl;
                    break;
                case VK_ERROR_OUT_OF_DEVICE_MEMORY:
                    std::cerr << "ODM" << std::endl;
                    break;
                case VK_ERROR_FRAGMENTED_POOL:
                    std::cerr << "FM" << std::endl;
                    break;
                case VK_ERROR_OUT_OF_POOL_MEMORY:
                    std::cerr << "OPM" << std::endl;
                    break;
                default:
                    break;
            }
            throw std::runtime_error("failed to allocate descriptor sets!");
        }

    }
};

