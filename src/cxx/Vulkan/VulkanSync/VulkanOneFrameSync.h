#pragma once

#include <vulkan/vulkan.h>
#include "../VulkanDevice/VulkanDevice.h"

class VulkanOneFrameSync
{
private:
    VkSemaphore availableSemaphore;
    VkSemaphore waitSemaphore;
    VulkanDevice *device;
    bool destroyed = false;
    VkFence fence;
public:
    VulkanOneFrameSync(VulkanDevice *device) : device(device)
    {
        createSyncObjects();
    }

    void destroy()
    {
        if (!destroyed)
        {
            vkDeviceWaitIdle(device->getDevice());
            vkDestroySemaphore(device->getDevice(), availableSemaphore, nullptr);
            vkDestroySemaphore(device->getDevice(), waitSemaphore, nullptr);
        }
        destroyed = true;
    }

    void submitCommandBuffer(VkCommandBuffer commandBuffer)
    {
        vkDeviceWaitIdle(device->getDevice());
        VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
        VkSubmitInfo submitInfo = {};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = &waitSemaphore;
        submitInfo.pSignalSemaphores = &availableSemaphore;
        submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &commandBuffer;
        submitInfo.pWaitDstStageMask = waitStages;
        vkQueueSubmit(device->getGraphicsQueue(), 1, &submitInfo, VK_NULL_HANDLE);
        std::swap(availableSemaphore, waitSemaphore);
    }

    ~VulkanOneFrameSync()
    {
        if (!destroyed)
        {
            destroy();
        }
    }

private:
    void createSyncObjects()
    {
        VkSemaphoreCreateInfo semaphoreInfo = {};
        semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
        vkCreateSemaphore(device->getDevice(), &semaphoreInfo, nullptr, &availableSemaphore);
        vkCreateSemaphore(device->getDevice(), &semaphoreInfo, nullptr, &waitSemaphore);
    }
};