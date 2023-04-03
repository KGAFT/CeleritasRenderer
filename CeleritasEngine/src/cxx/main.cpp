#include "Window/Window.h"
#include "Window/Monitor.h"
#include "Engine/Engine.h"
#include <glm/glm.hpp>
#include <array>


int main() {
    Window::initializeContext();
    int monitorCount = 0;
    Monitor **monitors = Monitor::enumerateMonitors(&monitorCount);

    Window::createWindow(1024, 768, "Hello world!", nullptr);
    Engine::createInstance("HelloWorld", false);
    EngineDevice device = Engine::enumSupportedDevices(Window::getInstance())[0];
    std::cout<<device.name<<std::endl;
    Engine engine(device, Window::getInstance());
    while (!Window::getInstance()->needToClose()) {
        Window::getInstance()->preRenderEvents();

        engine.update();

        Window::getInstance()->postRenderEvents();
    }
    return 0;
}
