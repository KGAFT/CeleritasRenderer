#include <iostream>
#include "Window/Window.h"
#include "DirectXContext/DirectXDevice.h"
#include "DirectXContext/DirectXSwapChain.h"
#include "DirectXContext/DirectXFrameBuffer.h"
int main() {
    Window::initializeContext();
    int monitorsAmount = 0;
    Monitor** monitors = Monitor::enumerateMonitors(&monitorsAmount);
    Window::createWindow(800, 600, "HelloWindow", nullptr);
    DirectXDevice::initializeDXContext();
    std::vector<TargetDevice> devices;
    DirectXDevice::enumerateSupportedDevices(devices);
    DirectXDevice* device = DirectXDevice::createDevice(devices[0]);
    DirectXSwapChain swapChain(device, Window::getInstance());
    ID3D11Texture2D* swapChainTexture = swapChain.acquireSwapChainTexture();
    DirectXFrameBuffer frameBuffer(&swapChainTexture, device, 1, true);
    frameBuffer.bind();

    while(!Window::getInstance()->needToClose()){
        frameBuffer.clear(0.5f, 0.5f, 0.8f, 1);
        swapChain.present(true);
        Window::getInstance()->postRenderEvents();
    }
    return 0;
}
