//
// Created by kgaft on 6/3/23.
//

#pragma once

#include <Vulkan/VulkanInstance.h>
#include "RenderPipelineImplementation/AssemblyPipeline.h"
#include "ShadowManager.h"
#include "RenderPipelineImplementation/GBufferPipeline.h"
#include "RenderPipelineImplementation/GameAssebmlyPipeline.h"
#include "../../TestKeyboardCallback.h"
#include <Vulkan/VulkanDevice/VulkanDevice.h>
#include <Vulkan/VulkanSwapChain.h>
#include <Vulkan/VulkanLogger/DefaultVulkanLoggerCallback.h>
#include <Vulkan/VulkanImage/VulkanImage.h>

namespace RenderingEngine{
    struct EngineDevice {
        std::string name;
        VkPhysicalDevice device;
        VkPhysicalDeviceProperties properties;
    };

    class Engine : public IWindowResizeCallback{
    private:
        static inline VulkanInstance *instance = nullptr;
        static inline bool debugBuild = false;
    public:
        static void createInstance(const char *appName, bool debugBuild) {
            instance = new VulkanInstance;
            Engine::debugBuild = debugBuild;
            uint32_t extCount;
            const char **exts = glfwGetRequiredInstanceExtensions(&extCount);
            std::vector<const char *> extensions;
            for (int i = 0; i < extCount; ++i) {
                extensions.push_back(exts[i]);
            }


            if (!instance->createInstance(appName, debugBuild, extensions)) {
                throw std::runtime_error("Failed to init Vulkan API");
            }
            if (debugBuild) {
                VulkanLogger::registerCallback(new DefaultVulkanLoggerCallback);
            }
        }

        static void enumSupportedDevices(Window *window, std::vector<EngineDevice>& result) {
            if (instance == nullptr) {
                throw std::runtime_error("Error: you need to create instance firstly");
            }
            for (const auto &item: VulkanDevice::enumerateSupportedDevices(instance->getInstance(),
                                                                           window->getWindowSurface(
                                                                                   instance->getInstance()))) {
                result.push_back({item.second.deviceName, item.first, item.second});
            }
        }
    private:
        AssemblyPipeline *asmPipeline;
        GBufferPipeline *gBufferPipeline;
        GameAssemblyPipeline* gameAssemblyPipeline;
        CameraManager *manager;
        ShadowManager* shadowManager;
        VulkanDevice *device;
        std::vector<Mesh*> registeredMeshes;
        Window *window;
        VulkanSwapChain *swapChain;
    public:
        Engine(EngineDevice &deviceToCreate, Window *window) : window(window) {
            if (instance == nullptr) {
                throw std::runtime_error("Error: you need to create instance firstly");
            }
            device = new VulkanDevice(deviceToCreate.device, window->getWindowSurface(instance->getInstance()),
                                      instance->getInstance(), debugBuild);
            swapChain = new VulkanSwapChain(device, window->getWidth(), window->getHeight());
            gBufferPipeline = new GBufferPipeline(device, window->getWidth(), window->getHeight());
            gameAssemblyPipeline = new GameAssemblyPipeline(device, window->getWidth(), window->getHeight());
            shadowManager = new ShadowManager(device, 4096, window->getWidth(), window->getHeight());
            asmPipeline = new AssemblyPipeline(device, swapChain, window->getWidth(), window->getHeight());
            manager = new CameraManager(gBufferPipeline->getCameraData());
            gameAssemblyPipeline->setGBufferPipeline(gBufferPipeline);
            gameAssemblyPipeline->setAo(shadowManager->getOutput());
            asmPipeline->setGamePlaceHolder(gameAssemblyPipeline->getOutputImages()[0]);
            asmPipeline->confirmPlaceHolders();
            window->registerResizeCallback(this);
            window->registerKeyCallback(new TestKeyboardCallback(window));
        }

        void registerMesh(Mesh* mesh){
            gBufferPipeline->registerMaterial(mesh->getMaterial());
            shadowManager->registerMesh(mesh);
            registeredMeshes.push_back(mesh);
        }

        void unRegisterMesh(Mesh* mesh){
            gBufferPipeline->unRegisterMaterial(mesh->getMaterial());
            shadowManager->unRegisterMesh(mesh);
        }

        void setSkybox(VulkanCubemapImage* skyboxImage){
            gameAssemblyPipeline->setSkyBox(skyboxImage);

        }

        void drawRegisteredMeshes(){
            manager->update();
            gBufferPipeline->beginRender();
            shadowManager->beginShadowPass();
            for (const auto &item: registeredMeshes){
                item->updateWorldMatrix();
                gBufferPipeline->processMesh(item);
                shadowManager->processMesh(item);
            }
            gBufferPipeline->endRender();
            shadowManager->endShadowPass(manager->getData()->viewMatrix, manager->getData()->cameraPosition);

            gameAssemblyPipeline->update();
            asmPipeline->getData().mode = 1;
            asmPipeline->update();
        }

        VulkanDevice *getDevice() const {
            return device;
        }

        void resized(int width, int height) override {
            vkDeviceWaitIdle(device->getDevice());
            gBufferPipeline->resize(width,height);
            gameAssemblyPipeline->resize(width,height);
            shadowManager->resizeOutput(width, height);
            gameAssemblyPipeline->setGBufferPipeline(gBufferPipeline);
            gameAssemblyPipeline->setAo(shadowManager->getOutput());
            asmPipeline->resize(width, height);
            asmPipeline->setGamePlaceHolder(gameAssemblyPipeline->getOutputImages()[0]);
            asmPipeline->setUiPlaceHolder(gameAssemblyPipeline->getOutputImages()[0]);
            asmPipeline->confirmPlaceHolders();
        }
    };
};