#include "Window/Window.h"
#include "Window/Monitor.h"
#include "Vulkan/VulkanInstance.h"
#include "Vulkan/VulkanDevice/VulkanDevice.h"
#include "Vulkan/VulkanSwapChain.h"
#include "Vulkan/VulkanRenderPass.h"
#include "Vulkan/VulkanSync/VulkanSyncManager.h"
#include "Util/ShaderLoader.h"
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
