#include "Window/Window.h"
#include "Window/Monitor.h"
#include "Vulkan/VulkanInstance.h"
#include "Vulkan/VulkanDevice/VulkanDevice.h"
#include "Vulkan/VulkanSwapChain.h"
#include "Vulkan/VulkanRenderPass.h"
#include "Vulkan/VulkanSync/VulkanSyncManager.h"
#include "Vulkan/VulkanGraphicsPipeline/PipelineEndConfiguration.h"
#include "Vulkan/VulkanGraphicsPipeline/GraphicsPipelineConfigurer.h"
#include "Vulkan/VulkanGraphicsPipeline/VulkanGraphicsPipeline.h"
#include "Util/ShaderLoader.h"
#include "Vulkan/VulkanRenderPipelineControl.h"
#include <glm/glm.hpp>
#include <array>

struct PushConstantData{
    glm::mat4 cameraMatrix;
    glm::mat4 modelMatrix;
    glm::vec3 cameraPosition;
};

int main() {
    Window::initializeContext();
    int monitorCount = 0;
    


    Monitor **monitors = Monitor::enumerateMonitors(&monitorCount);

    Window::createWindow(1024, 768, "Hello world!", nullptr);

    Window::getInstance()->borderless();
    Window::getInstance()->borderless();

    VulkanInstance instance;
    std::cout << instance.createInstance("CeleritasEngine", true) << std::endl;
    VkPhysicalDevice deviceToCreate;
    for (const auto &item: VulkanDevice::enumerateSupportedDevices(instance.getInstance(), Window::getInstance())) {
        std::cout << item.second.deviceName << std::endl;
        deviceToCreate = item.first;
        break;
    }
    VulkanDevice device(deviceToCreate, Window::getInstance(), instance.getInstance(), true);

    VulkanShader* shader = ShaderLoader::loadShaders("shaders/TestVulkanShader", &device);

    VulkanSwapChain swapChain(&device, Window::getInstance()->getWidth(), Window::getInstance()->getHeight());
    VkFormat format = swapChain.getSwapChainImageFormat();
    VulkanRenderPass renderPass(&device, swapChain.getSwapChainImageViews(), Window::getInstance()->getWidth(),
                                Window::getInstance()->getHeight(), 1, &format, 1);

    VulkanSyncManager syncManager(&device, &swapChain);
    VulkanSyncManager secondSyncManager(&device, nullptr);

    PipelineEndConfig endConfig{};
    endConfig.vertexInputs.push_back({0, 3, sizeof(float), VK_FORMAT_R32G32B32_SFLOAT});
    endConfig.vertexInputs.push_back({1, 4, sizeof(float), VK_FORMAT_R32G32B32A32_SFLOAT});
    endConfig.pushConstantInfos.push_back({VK_SHADER_STAGE_VERTEX_BIT, sizeof(PushConstantData)});
    endConfig.uniformBuffers.push_back({0, sizeof(glm::vec3), VK_SHADER_STAGE_VERTEX_BIT});
    GraphicsPipelineConfigurer configurer(&device, &endConfig);
    PipelineConfiguration::PipelineConfigInfo config = PipelineConfiguration::defaultPipelineConfigInfo(Window::getInstance()->getWidth(), Window::getInstance()->getHeight());
    VulkanGraphicsPipeline graphicsPipeline(&device, configurer, shader, config, &renderPass);

    VulkanRenderPipelineControl control(&syncManager, &device, &renderPass);

    while (!Window::getInstance()->needToClose()) {
        Window::getInstance()->preRenderEvents();
        std::pair rendInfo = control.beginRender();

        VkRenderPassBeginInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = renderPass.getRenderPass();
        renderPassInfo.framebuffer = rendInfo.second;
        renderPassInfo.renderArea.offset = {0, 0};
        renderPassInfo.renderArea.extent = {static_cast<uint32_t>(Window::getInstance()->getWidth()), static_cast<uint32_t>(Window::getInstance()->getHeight())};
        std::array<VkClearValue, 2> clearValues{};
        clearValues[0].color = {0.5f, 0.2f, 0.4f, 1.0f};
        clearValues[1].depthStencil = {1.0f, 0};
        renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
        renderPassInfo.pClearValues = clearValues.data();

        vkCmdBeginRenderPass(rendInfo.first, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
        vkCmdEndRenderPass(rendInfo.first);

        control.endRender();

        Window::getInstance()->postRenderEvents();
    }
    renderPass.destroy();
    syncManager.destroy();
    secondSyncManager.destroy();
    swapChain.destroy();

    delete shader;
    device.destroy();
    int wait = 0;
    std::cin>>wait;
    return 0;
}
