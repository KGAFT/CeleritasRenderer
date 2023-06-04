//
// Created by Daniil on 01.05.2023.
//
#pragma once
#include "RenderPipelineImplementation/DirectLightShadowBuffer.h"
#include "RenderPipelineImplementation/DirectLightShadowAssemblyPipeline.h"
namespace RenderingEngine{
    class ShadowManager{
    private:
        DirectLightShadowBuffer* shadowBufferPipeline;
        DirectLightShadowAssemblyPipeline* shadowAssemblyPipeline;
        VulkanDevice* device;
        VulkanImage* depthBuffer;
        VkCommandBuffer shadowCmd;
        std::vector<Mesh*> meshesBuffer;
    public:
        ShadowManager(VulkanDevice* device, int bufferQuadLen, int windowWidth, int windowHeight) : device(device){
            shadowBufferPipeline = new DirectLightShadowBuffer(device, bufferQuadLen, bufferQuadLen);
            depthBuffer = VulkanImage::createImageWithFormat(device, bufferQuadLen, bufferQuadLen, findDepthFormat());
            shadowAssemblyPipeline = new DirectLightShadowAssemblyPipeline(device, windowWidth, windowHeight);
            shadowAssemblyPipeline->setShadowMap(depthBuffer);

        }
        void setupLightView(glm::vec3 lightPosition, float maxLightDistance){
            shadowBufferPipeline->recalculateMatrixForLightSource(lightPosition, maxLightDistance);
            shadowAssemblyPipeline->getConfig().lightPosition = lightPosition;
        }

        void beginShadowPass(){
            shadowCmd = shadowBufferPipeline->beginRender();
        }

        void processMesh(Mesh* mesh){
            shadowBufferPipeline->processMesh(mesh);
            meshesBuffer.push_back(mesh);
        }

        void registerMesh(Mesh* mesh){
            shadowAssemblyPipeline->registerMeshMaterial(mesh->getMaterial());
        }

        void unRegisterMesh(Mesh* mesh){
            shadowAssemblyPipeline->unRegisterMeshMaterial(mesh->getMaterial());
        }

        void endShadowPass(glm::mat4 viewMatrix, glm::vec3 cameraPosition){
            depthBuffer->clearImage(0,0,0,0,shadowCmd);
            depthBuffer->copyFromImage(shadowBufferPipeline->getDepthImages()[0], shadowCmd);
            shadowBufferPipeline->endRender();
            shadowAssemblyPipeline->beginRender();
            for (const auto &item: meshesBuffer){
                shadowAssemblyPipeline->getConfig().normalMapEnabled = item->getMaterial()->getNormalMap()!=nullptr;
                shadowAssemblyPipeline->getConfig().previousAoEnabled = item->getMaterial()->getAoTexture()!=nullptr;
                shadowAssemblyPipeline->getWorldTransfomData().lightSpaceMatrix = shadowBufferPipeline->getViewData().lightSpaceMatrix;
                shadowAssemblyPipeline->getWorldTransfomData().viewMatrix = viewMatrix;
                shadowAssemblyPipeline->getWorldTransfomData().cameraPosition = cameraPosition;

                shadowAssemblyPipeline->processMesh(item);

            }
            shadowAssemblyPipeline->endRender();
            meshesBuffer.clear();
        }

        VulkanImage* getOutput(){
            return shadowAssemblyPipeline->getOutputImages()[0];
        }

        void resizeOutput(int width, int height){
            shadowAssemblyPipeline->resize(width, height);
        }

        void resizeInternal(int squareLen){
            shadowBufferPipeline->resize(squareLen, squareLen);
            depthBuffer->resize(squareLen, squareLen);
            shadowAssemblyPipeline->setShadowMap(depthBuffer);
        }
    private:
        VkFormat findDepthFormat() {
            return device->findSupportedFormat(
                    {VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT},
                    VK_IMAGE_TILING_OPTIMAL,
                    VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
        }
    };
}
