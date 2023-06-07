#include "Window/Window.h"
#include "Window/Monitor.h"
#include "Engine/RenderingEngine/RenderingEngine.h"
#include "Util/ModelLoader.h"
#include "Engine/RenderingEngineSecond/RenderingPipelines/AssemblyPipeline.h"
#include <glm/glm.hpp>
#include <array>


int main() {
    Window::initializeContext();
    int monitorCount = 0;
    Monitor **monitors = Monitor::enumerateMonitors(&monitorCount);

    Window::createWindow(800, 600, "Hello world!", nullptr);
    RenderingEngine::Engine::createInstance("HelloWorld", true);
    std::vector<RenderingEngine::EngineDevice> devices;
    RenderingEngine::Engine::enumSupportedDevices(Window::getInstance(), devices);
    int count = 1;
    std::cout<<"Choose device: "<<std::endl;
    std::cout<<"ID:\tDeviceName:"<<std::endl;
    for(int i = 0; i<devices.size(); i++){
        std::cout<<i<<"\t"<<devices[i].name<<std::endl;
        
    }
    std::cout<<"Enter device id: ";
   // std::cin>>count;
    count = 0;
    RenderingEngine::Engine engine(devices[count], Window::getInstance());
    RenderEngine::AssemblyPipeline assemblyPipeline(engine.getDevice(), engine.getSwapChain(), 800, 600);

    ModelLoader loader(engine.getDevice());
    std::vector<Mesh*> helmetMeshes = loader.loadModel("models/Helmet/DamagedHelmet.gltf", true);
    VulkanImage *albedoIm = VulkanImage::loadTexture("models/Helmet/Default_albedo.jpg", engine.getDevice());
    for (const auto &item: helmetMeshes) {
        item->getMaterial()->setAoTexture(nullptr);
        item->getMaterial()->setOpacityMapTexture(nullptr);
        item->getMaterial()->setAlbedoTexture(albedoIm);
        item->setPosition(glm::vec3(0, 0, 10));
        item->setScale(glm::vec3(5, 5, 5));
        engine.registerMesh(item);
    }
    loader.clear();
    CubemapTextureInfo cubeMapInfo{};
    cubeMapInfo.pathToFrontFace = "models/SkyBox/cloudy/bluecloud_ft.jpg";
    cubeMapInfo.pathToBackFace = "models/SkyBox/cloudy/bluecloud_bk.jpg";
    cubeMapInfo.pathToUpFace = "models/SkyBox/cloudy/bluecloud_up.jpg";
    cubeMapInfo.pathToDownFace = "models/SkyBox/cloudy/bluecloud_lf.jpg";
    cubeMapInfo.pathToLeftFace = "models/SkyBox/cloudy/bluecloud_lf.jpg";
    cubeMapInfo.pathToRightFace = "models/SkyBox/cloudy/bluecloud_ft.jpg";
    VulkanCubemapImage* skyBox = VulkanCubemapImage::createCubemap(engine.getDevice(), cubeMapInfo);

    engine.setSkybox(skyBox);

    while (!Window::getInstance()->needToClose()) {
        Window::getInstance()->preRenderEvents();

        engine.drawRegisteredMeshes();

        Window::getInstance()->postRenderEvents();
    }
    return 0;
}
