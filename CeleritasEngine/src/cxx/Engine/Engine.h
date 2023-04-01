//
// Created by KGAFT on 3/17/2023.
//
#pragma once
#include "../Vulkan/VulkanInstance.h"
#include "../Window/Window.h"
#include "../Vulkan/VulkanDevice/VulkanDevice.h"
#include "../Vulkan/VulkanSwapChain.h"
#include "../Vulkan/VulkanLogger/DefaultVulkanLoggerCallback.h"
#include "Pipelines/AssemblyPipeline/AssemblyPipeline.h"
#include "../Vulkan/VulkanImage/VulkanImage.h"
#include "Pipelines/UIPipeline/UIPipeline.h"
#include "Pipelines/GamePipeline/GBufferPipeline.h"
#include "../Util/ModelLoader.h"
#include "Camera/CameraManager.h"

#include "../TestKeyboardCallback.h"

struct EngineDevice
{
    std::string name;
    VkPhysicalDevice device;
    VkPhysicalDeviceProperties properties;
};

class Engine
{
private:
    static inline VulkanInstance *instance = nullptr;
    static inline bool debugBuild = false;

public:
    static void createInstance(const char *appName, bool debugBuild)
    {
        instance = new VulkanInstance;
        Engine::debugBuild = debugBuild;
        if (!instance->createInstance(appName, debugBuild))
        {
            throw std::runtime_error("Failed to init Vulkan API");
        }
        if (debugBuild)
        {
            VulkanLogger::registerCallback(new DefaultVulkanLoggerCallback);
        }
    }
    static std::vector<EngineDevice> enumSupportedDevices(Window *window)
    {
        if (instance == nullptr)
        {
            throw std::runtime_error("Error: you need to create instance firstly");
        }
        std::vector<EngineDevice> result;
        for (const auto &item : VulkanDevice::enumerateSupportedDevices(instance->getInstance(), window))
        {
            result.push_back({item.second.deviceName, item.first, item.second});
        }
        return result;
    }

private:
    VulkanDevice *device;
    Window *window;
    VulkanSwapChain *swapChain;
    AssemblyPipeline *assemblyPipeline;
    VulkanImage *uiPlaceHolder;
    VulkanImage *gamePlaceHolder;
    UIPipeline *uiPipeline;
   // GBufferPipeline *gBufferPipeline;
    CameraManager manager;
    std::vector<Mesh *> meshes;
    PushConstantData pcData;
    Material material;

public:
    Engine(EngineDevice &deviceToCreate, Window *window) : window(window), manager(&pcData)
    {
        if (instance == nullptr)
        {
            throw std::runtime_error("Error: you need to create instance firstly");
        }
        device = new VulkanDevice(deviceToCreate.device, window, instance->getInstance(), debugBuild);
        swapChain = new VulkanSwapChain(device, window->getWidth(), window->getHeight());
        // uiPlaceHolder = AssetLoader::loadTextures(device, "C:/Users/Daniil/Desktop/model.sc");

        ModelLoader loader(device);
        meshes = loader.loadModel("C:/Users/Daniil/Desktop/vaze/Vaze.fbx");

        material.setAlbedoTexture(VulkanImage::loadTexture("C:/Users/Daniil/Desktop/vaze/albedo.tga", device));
        material.setMetallicTexture(VulkanImage::loadTexture("C:/Users/Daniil/Desktop/vaze/metallic.tga", device));
        material.setRoughnessTexture(VulkanImage::loadTexture("C:/Users/Daniil/Desktop/vaze/roughness.tga", device));
        material.setNormalMap(VulkanImage::loadTexture("C:/Users/Daniil/Desktop/vaze/normal.tga", device));
        meshes[0]->setMaterial(&material);
        TestKeyboardCallback *keyBoardCB = new TestKeyboardCallback(Window::getInstance());
        Window::getInstance()->registerKeyCallback(keyBoardCB);
        uiPlaceHolder = VulkanImage::loadTexture("shaders/ui.png", device);
        gamePlaceHolder = VulkanImage::loadTexture("shaders/ui.png", device);
      //  gBufferPipeline = new GBufferPipeline(device, Window::getInstance()->getWidth(), Window::getInstance()->getWidth());

        assemblyPipeline = new AssemblyPipeline(device, swapChain, window);

        assemblyPipeline->getCorrect().outCorrectAmplifier.a = 0.5f;

        uiPipeline = new UIPipeline(device, Window::getInstance()->getWidth(), Window::getInstance()->getHeight());
        assemblyPipeline->getGameSampler()->setSamplerImageView(uiPlaceHolder->getView());
        assemblyPipeline->getUISampler()->setSamplerImageView(uiPlaceHolder->getView());
    }

    void update()
    {
        VkImageView imageView = uiPipeline->update();

        manager.update();
        /*
        gBufferPipeline->getViewData().cameraPosition = pcData.cameraPosition;
        gBufferPipeline->getViewData().viewMatrix = pcData.cameraMatrix;
        gBufferPipeline->getViewData().worldMatrix = pcData.modelMatrix;

        VkCommandBuffer cmd = gBufferPipeline->beginRender();
        for (const auto &el : meshes)
        {
            glm::mat4 modelMat(1.0f);
            glm::translate(el->getPosition());
            glm::scale(el->getScale());
            glm::rotate(el->getRotation().x, glm::vec3(1, 0, 0));
            glm::rotate(el->getRotation().y, glm::vec3(0, 1, 0));
            glm::rotate(el->getRotation().z, glm::vec3(0, 0, 1));
            gBufferPipeline->getViewData().worldMatrix = modelMat;
            gBufferPipeline->populateSamplers(el->getMaterial());
            gBufferPipeline->updatePcs();
            gBufferPipeline->updateUniforms();
            el->draw(cmd);
        }
        gBufferPipeline->endRender();
         */

       // assemblyPipeline->getUISampler()->setSamplerImageView(imageView);
        //assemblyPipeline->getGameSampler()->setSamplerImageView(imageView);
        assemblyPipeline->update();
    }
};