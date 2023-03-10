#include "Window/Window.h"
#include "Window/Monitor.h"
#include "Vulkan/VulkanInstance.h"
#include "Vulkan/VulkanDevice/VulkanDevice.h"
int main() {
    Window::initializeContext();
    int monitorCount = 0;
    Monitor** monitors = Monitor::enumerateMonitors(&monitorCount);
    
    Window::createWindow(1024, 768, "Hello world!", nullptr);

    Window::getInstance()->borderless();
    Window::getInstance()->borderless();

    VulkanInstance instance;
    std::cout<<instance.createInstance("CeleritasEngine", true)<<std::endl;
    for (const auto &item: VulkanDevice::enumerateSupportedDevices(instance.getInstance(), Window::getInstance())){
        std::cout<<item.second.deviceName<<std::endl;
    }
    while (!Window::getInstance()->needToClose()) {
        Window::getInstance()->preRenderEvents();
        Window::getInstance()->postRenderEvents();
    }
    return 0;
}
