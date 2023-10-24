#include "Window/Window.hpp"
#include "Window/Monitor.hpp"
#include "Util/ModelLoader.h"
#include <glm/glm.hpp>
#include <array>
#include "Engine/RenderingEngine/RenderingEngine.h"

int main() {
    glfwInit();
    Window* window = Window::createWindow(800, 600, "CeleritasEngine", nullptr, false);
    RenderEngine::Engine::initializeContexts("CeleritasEngine", window, false);
    std::vector<RenderEngine::EngineDevice> devices;

    RenderEngine::Engine::enumerateSupportedDevice(devices);
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
       // std::cin>>deviceId;
    }
    std::cout<<"Choosed device for rendering: "<<devices[0].name<<std::endl;
    RenderEngine::Engine engine(devices[0], window);
    window->addResizeCallback(&engine);

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
        item->setPosition(glm::vec3(10, 0, 0));
        item->setScale(glm::vec3(5, 5, 5));
        engine.registerMesh(item);
    }
    loader.clear();
    Mesh* cubeMesh = acquireSkyboxMesh(engine.getDevice());
    cubeMesh->setPosition(glm::vec3(1,0,0));
    cubeMesh->setMaterial(engine.getMeshesToDraw()[0]->getMaterial());
    engine.registerMesh(cubeMesh);
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
