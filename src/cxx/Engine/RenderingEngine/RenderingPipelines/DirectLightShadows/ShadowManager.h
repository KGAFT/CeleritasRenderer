//
// Created by Daniil on 01.05.2023.
//
#pragma once
#include "DirectLightShadowBuffer.h"
#include "DirectLightShadowAssemblyPipeline.h"
namespace RenderEngine
{
    class ShadowManager
    {
    public:
        ShadowManager(VulkanDevice *device, int bufferQuadLen, int windowWidth, int windowHeight);

    private:
        DirectLightShadowBuffer *shadowBufferPipeline;
        DirectLightShadowAssemblyPipeline *shadowAssemblyPipeline;
        VulkanDevice *device;
        VulkanImage *depthBuffer;
        VkCommandBuffer shadowCmd;
        std::vector<Mesh *> meshesBuffer;
    public:
        void setupLightView(glm::vec3 lightPosition, float maxLightDistance);

        void beginShadowPass();

        void processMesh(Mesh *mesh);

        void registerMesh(Mesh *mesh);

        void unRegisterMesh(Mesh *mesh);

        void endShadowPass(glm::mat4 viewMatrix, glm::vec3 cameraPosition);

        VulkanImage *getOutput();

        void resizeOutput(int width, int height);

        void resizeInternal(int squareLen);

    private:
        VkFormat findDepthFormat();
    };
}
