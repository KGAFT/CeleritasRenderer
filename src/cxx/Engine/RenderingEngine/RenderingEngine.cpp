//
// Created by kgaft on 6/7/23.
//

#include "RenderingEngine.h"

using namespace RenderEngine;

VulkanInstance* Engine::instance = nullptr;
bool Engine::debugBuild = false;
Window* Engine::targetWindow = nullptr;

void Engine::initializeContexts(const char* appName, Window* window, bool debugBuild){
    if(Engine::instance==nullptr){
        std::vector<const char*> windowExts;
        uint32_t extCount;
        const char** exts = glfwGetRequiredInstanceExtensions(&extCount);
        for(uint32_t c = 0; c<extCount; c++){
            windowExts.push_back(exts[c]);
        }

        Engine::instance = new VulkanInstance();
        VulkanLogger::registerCallback(new DefaultVulkanLoggerCallback());
        Engine::instance->createInstance(appName, debugBuild, windowExts);
        
        Engine::debugBuild = debugBuild;
        targetWindow = window;
    }
}

void Engine::enumerateSupportedDevice(std::vector<EngineDevice>& output){
    if(Engine::instance==nullptr){
        throw std::runtime_error("Error: you need to initialize contexts firstly!");
    }
    std::map<VkPhysicalDevice, VkPhysicalDeviceProperties> devices = VulkanDevice::enumerateSupportedDevices(instance->getInstance(), targetWindow->getWindowSurface(instance->getInstance()));
    for(const auto& element : devices){
        output.push_back({element.second.deviceName, element.first});
    }
}

RenderEngine::Engine::Engine(EngineDevice &targetDevice)
{
    if(instance == nullptr){
        throw std::runtime_error("Failed to create engine object, you need to initialize context firstly!");
    }
    device = new VulkanDevice(targetDevice.base, targetWindow->getWindowSurface(instance->getInstance()), instance->getInstance(), debugBuild);
    swapChain = new VulkanSwapChain(device, targetWindow->getWidth(), targetWindow->getHeight());
    assemblyPipeline = new AssemblyPipeline(device, swapChain, targetWindow->getWidth(), targetWindow->getHeight());
    gameAssemblyPipeline = new GameAssemblyPipeline(device, targetWindow->getWidth(), targetWindow->getHeight());
    gBufferPipeline = new GBufferPipeline(device, targetWindow->getWidth(), targetWindow->getHeight());
    skyboxPipeline = new SkyboxPipeline(device, targetWindow->getWidth(), targetWindow->getHeight());
    shadowManager = new ShadowManager(device, 1024, targetWindow->getWidth(), targetWindow->getHeight());
    cameraManager = new CameraManager(gBufferPipeline->getWorldTransformData());
    targetWindow->registerKeyCallback(new TestKeyboardCallback(targetWindow));
    shadowManager->setupLightView(glm::vec3(-5,-8.0f,5.0f), 200);
    setupPipelinesConnections();
}

void RenderEngine::Engine::setSkybox(VulkanCubemapImage *cubemap)
{
    gameAssemblyPipeline->setReflectionImage(cubemap);
    skyboxPipeline->setSkyboxImage(cubemap);
    gameAssemblyPipeline->confirmInputs();
}

void RenderEngine::Engine::resizeOutput(int width, int height)
{
    vkDeviceWaitIdle(device->getDevice());
    gBufferPipeline->resize(width, height);
    skyboxPipeline->resize(width, height);
    shadowManager->resizeOutput(width, height);

    gameAssemblyPipeline->resize(width, height);
    gameAssemblyPipeline->setGBuffer(gBufferPipeline);
    gameAssemblyPipeline->setAo(shadowManager->getOutput());
    gameAssemblyPipeline->setBackground(skyboxPipeline->getOutput());
    gameAssemblyPipeline->confirmInputs();

    assemblyPipeline->resize(width, height);
    assemblyPipeline->setGamePlaceHolder(gameAssemblyPipeline->getOutput());
}

void RenderEngine::Engine::resized(int width, int height)
{
    resizeOutput(width, height);
}

void RenderEngine::Engine::registerMesh(Mesh *mesh)
{
    meshesToDraw.push_back(mesh);
    gBufferPipeline->registerMaterial(mesh->getMaterial());
    shadowManager->registerMesh(mesh);

}

void RenderEngine::Engine::drawRegisteredMeshes()
{
    cameraManager->update();
    gBufferPipeline->beginRender();
    shadowManager->beginShadowPass();
    for(const auto& mesh : meshesToDraw){
        mesh->updateWorldMatrix();
        gBufferPipeline->drawMesh(mesh);
        shadowManager->processMesh(mesh);
    }
    gBufferPipeline->endRender();
    shadowManager->endShadowPass(cameraManager->getData()->viewMatrix, cameraManager->getData()->cameraPosition);
    skyboxPipeline->update(cameraManager->getData()->viewMatrix);
    gameAssemblyPipeline->getVertexConfig().cameraPosition = cameraManager->getData()->cameraPosition;
    gameAssemblyPipeline->update();
    assemblyPipeline->update();
}

VulkanDevice *RenderEngine::Engine::getDevice()
{
    return device;
}

LightConfiguration &RenderEngine::Engine::getLightConfig()
{
    return gameAssemblyPipeline->getLightConfig();
}

void RenderEngine::Engine::setupPipelinesConnections()
{
    gameAssemblyPipeline->setGBuffer(gBufferPipeline);
    gameAssemblyPipeline->setAo(shadowManager->getOutput());
    gameAssemblyPipeline->setBackground(skyboxPipeline->getOutput());
    gameAssemblyPipeline->confirmInputs();
    assemblyPipeline->setGamePlaceHolder(gameAssemblyPipeline->getOutput());
}
