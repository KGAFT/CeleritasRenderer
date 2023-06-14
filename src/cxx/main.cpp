#include "Window/Window.h"
#include "Window/Monitor.h"
#include "Util/ModelLoader.h"
#include <glm/glm.hpp>
#include <array>
#include "Engine/RenderingEngine/RenderingEngine.h"

int main() {
    Window::initializeContext();
    Window* window = Window::createWindow(1680, 1060, "CeleritasEngine", nullptr);
    RenderEngine::Engine::initializeContexts("CeleritasEngine", window, true);
    std::vector<RenderEngine::EngineDevice> devices;

    RenderEngine::Engine::enumerateSupportedDevice(devices);
    RenderEngine::EngineDevice* targetDevice;
    int deviceId = 0;
    if(devices.size()>1){
        std::cout<<"The engine has found several graphics adapters, please choose which one you wanna use!"<<std::endl;
        std::cout<<"ID:\tDevice name:"<<std::endl;
        int count = 0;
        for(const auto& el :devices){
            std::cout<<count<<"\t"<<el.name<<std::endl;
            count++;
        }
        std::cout<<"Please enter specified device id: ";
        std::cin>>deviceId;
    }
    std::cout<<"Choosed device for rendering: "<<devices[deviceId].name<<std::endl;
    
    return 0;
}
