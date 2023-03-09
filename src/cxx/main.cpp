#include "Window/Window.h"
#include "Window/Monitor.h"
#include "TestKeyboardCallback.h"
#include "TestMouseCallback.h"
int main() {
    Window::initializeContext();
    int monitorCount = 0;
    Monitor** monitors = Monitor::enumerateMonitors(&monitorCount);
    
    Window::createWindow(1024, 768, "Hello world!", nullptr);

    Window::getInstance()->borderless();
    Window::getInstance()->borderless();
    TestKeyboardCallback keyBoard(Window::getInstance());
    TestMouseCallbacK* mouseCb = new TestMouseCallbacK();
    Window::getInstance()->registerKeyCallback(&keyBoard);
    Window::getInstance()->registerMouseCallback(mouseCb);
    while (!Window::getInstance()->needToClose()) {
        Window::getInstance()->preRenderEvents();
        Window::getInstance()->postRenderEvents();
    }
    return 0;
}
