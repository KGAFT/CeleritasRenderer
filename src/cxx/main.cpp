#include "Window/Window.h"
#include "Window/Monitor.h"
#include "Vulkan/VulkanInstance.h"
#include "Vulkan/VulkanDevice/VulkanDevice.h"
#include "Vulkan/VulkanSwapChain.h"
#include "Vulkan/VulkanRenderingPipeline/VulkanRenderPass.h"
#include "Vulkan/VulkanSync/VulkanSyncManager.h"
#include "Vulkan/VulkanGraphicsPipeline/PipelineEndConfiguration.h"
#include "Vulkan/VulkanGraphicsPipeline/GraphicsPipelineConfigurer.h"
#include "Vulkan/VulkanGraphicsPipeline/VulkanGraphicsPipeline.h"
#include "Util/ShaderLoader.h"
#include "Vulkan/VulkanRenderingPipeline/VulkanRenderPipelineControl.h"
#include "Vulkan/VulkanBuffers/VertexBuffer.h"
#include "Vulkan/VulkanImmediateShaderData/PushConstants/VulkanPushConstant.h"
#include "Vulkan/VulkanImmediateShaderData/PushConstants/VulkanPushConstantManager.h"
#include "Vulkan/VulkanEndRenderPipeline.h"
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


    VulkanSyncManager syncManager(&device, &swapChain);

    PipelineEndConfig endConfig{};
    endConfig.vertexInputs.push_back({0, 3, sizeof(float), VK_FORMAT_R32G32B32_SFLOAT});
    endConfig.vertexInputs.push_back({1, 4, sizeof(float), VK_FORMAT_R32G32B32A32_SFLOAT});



    float vertices[]{
        0.0f,0.0f,0.0f,     0.0f,0.0f,0.0f, 1.0f,
        0.0f, 0.5f, 0.0f,   0.0f, 0.5f, 0.0f, 1.0f,
        0.5f, 0.0f, 0.0f,   0.5f, 0.0f, 0.0f, 1.0f
    };

    VertexBuffer vertexBuffer(7*sizeof(float), 3, &device, vertices);
    VulkanEndRenderPipeline endRenderPipeline(&device, &syncManager, shader, &endConfig, Window::getInstance()->getWidth(), Window::getInstance()->getHeight(), swapChain.getSwapChainImageViews(), 1, swapChain.getSwapChainImageFormat());
    Window::getInstance()->registerResizeCallback(&endRenderPipeline);


    while (!Window::getInstance()->needToClose()) {
        Window::getInstance()->preRenderEvents();
        VkCommandBuffer commandBuffer = endRenderPipeline.beginRender();
        vertexBuffer.bind(commandBuffer);
        vertexBuffer.draw(commandBuffer);
        endRenderPipeline.endRender();


        Window::getInstance()->postRenderEvents();
    }

    endRenderPipeline.destroy();
    vertexBuffer.destroy();
    syncManager.destroy();

    swapChain.destroy();

    delete shader;
    device.destroy();
    int wait = 0;
    std::cin>>wait;
    return 0;
}
