#include "Window/Window.h"
#include "Window/Monitor.h"
#include "Util/ModelLoader.h"
#include <glm/glm.hpp>
#include <array>
#include "Engine/RenderingEngine/RenderingEngine.h"

int main() {
    Window::initializeContext();
    Window* window = Window::createWindow(1680, 1060, "CeleritasEngine", nullptr);
    RenderEngine::Engine::initializeContexts("CeleritasEngine", window, false);
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
    RenderEngine::Engine engine(devices[deviceId]);
    window->registerResizeCallback(&engine);

    CubemapTextureInfo cubeMapInfo{};
    cubeMapInfo.pathToFrontFace = "models/SkyBox/cloudy/bluecloud_ft.jpg";
    cubeMapInfo.pathToBackFace = "models/SkyBox/cloudy/bluecloud_bk.jpg";
    cubeMapInfo.pathToUpFace = "models/SkyBox/cloudy/bluecloud_up.jpg";
    cubeMapInfo.pathToDownFace = "models/SkyBox/cloudy/bluecloud_lf.jpg";
    cubeMapInfo.pathToLeftFace = "models/SkyBox/cloudy/bluecloud_lf.jpg";
    cubeMapInfo.pathToRightFace = "models/SkyBox/cloudy/bluecloud_ft.jpg";
    engine.setSkybox(VulkanCubemapImage::createCubemap(engine.getDevice(), cubeMapInfo));

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

    
    engine.getLightConfig().enabledDirects = 1;
    engine.getLightConfig().directLights[0].color = glm::vec3(1, 1, 1);
    engine.getLightConfig().directLights[0].direction = glm::vec3(0, 5, -5);
    engine.getLightConfig().directLights[0].intensity = 5;
    engine.getLightConfig().emissiveShininess = 2;
    while(!window->needToClose()){
        window->preRenderEvents();
        engine.drawRegisteredMeshes();
        window->postRenderEvents();
    }
    return 0;
}
