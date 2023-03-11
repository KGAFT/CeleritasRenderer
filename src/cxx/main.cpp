#include "Window/Window.h"
#include "Window/Monitor.h"
#include "Vulkan/VulkanInstance.h"
#include "Vulkan/VulkanDevice/VulkanDevice.h"
#include "Vulkan/VulkanSwapChain.h"
#include "Vulkan/VulkanRenderPass.h"
#include "Vulkan/VulkanSync/VulkanOneFrameSync.h"
#include "Vulkan/VulkanSync/VulkanThreeFrameSync.h"
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
    VulkanSwapChain swapChain(&device, Window::getInstance()->getWidth(), Window::getInstance()->getHeight());
    VkFormat format = swapChain.getSwapChainImageFormat();
    VulkanRenderPass renderPass(&device, swapChain.getSwapChainImageViews(), Window::getInstance()->getWidth(),
                                Window::getInstance()->getHeight(), 1, &format, 1);
    VulkanOneFrameSync oneFrameSync(&device);
    VulkanThreeFrameSync threeFrameSync(&device);
    while (!Window::getInstance()->needToClose()) {
        Window::getInstance()->preRenderEvents();
        Window::getInstance()->postRenderEvents();
    }
    renderPass.destroy();
    swapChain.destroy();
    threeFrameSync.destroy();
    oneFrameSync.destroy();
    device.destroy();
    int wait = 0;
    std::cin>>wait;
    return 0;
}
