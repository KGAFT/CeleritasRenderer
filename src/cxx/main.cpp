#include "Window/Window.h"
#include "Window/Monitor.h"
int main() {
    Window::initializeContext();
    int monitorCount = 0;
    Monitor** monitors = Monitor::enumerateMonitors(&monitorCount);
    
    Window::createWindow(1024, 768, "Hello world!", nullptr);

    Window::getInstance()->setMonitor(monitors[1]);
    while (!Window::getInstance()->needToClose()) {
        Window::getInstance()->postRenderEvents();
    }
    return 0;
}
