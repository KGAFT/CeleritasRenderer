#include "Window/Window.h"
#include "Window/Monitor.h"
#include "Engine/Engine.h"
#include <glm/glm.hpp>
#include <array>


int main() {
    Window::initializeContext();
    int monitorCount = 0;
    Monitor **monitors = Monitor::enumerateMonitors(&monitorCount);

    Window::createWindow(800, 600, "Hello world!", nullptr);
    Engine::createInstance("HelloWorld", true);
    std::vector<EngineDevice> devices;
    Engine::enumSupportedDevices(Window::getInstance(), devices);
    int count = 1;
    std::cout<<"Choose device: "<<std::endl;
    std::cout<<"ID:\tDeviceName:"<<std::endl;
    for(int i = 0; i<devices.size(); i++){
        std::cout<<i<<"\t"<<devices[i].name<<std::endl;
        
    }
    std::cout<<"Enter device id: ";
    cin>>count;

    Engine engine(devices[count], Window::getInstance());
    while (!Window::getInstance()->needToClose()) {
        Window::getInstance()->preRenderEvents();

        engine.update();

        Window::getInstance()->postRenderEvents();
    }
    return 0;
}
