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
#include <glm/glm.hpp>
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
    while (!Window::getInstance()->needToClose()) {
        Window::getInstance()->preRenderEvents();
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
