#pragma once
#include "Pipelines/ShadowBufferPipeline.h"
#include "Pipelines/ShadowAssemblyPipeline.h"
class ShadowManager{
private:
        ShadowBufferPipeline* shadowBufferPipeline;
        ShadowAssemblyPipeline* shadowAssemblyPipeline;
        VulkanDevice* device;
        VulkanImage* depthBuffer;
public:
    ShadowManager(VulkanDevice* device, int bufferQuadLen, int windowWidth, int windowHeight) : device(device){
        shadowBufferPipeline = new ShadowBufferPipeline(device, bufferQuadLen, bufferQuadLen);
        depthBuffer = VulkanImage::createImageWithFormat(device, bufferQuadLen, bufferQuadLen, findDepthFormat());
        shadowAssemblyPipeline = new ShadowAssemblyPipeline(device, windowWidth, windowHeight);
        shadowAssemblyPipeline->setShadowMap(depthBuffer->getView());
        shadowAssemblyPipeline->updateSamplers();
    }
    void setupLightView(glm::vec3 lightPosition, float maxLightDistance){
        shadowBufferPipeline->recalculateMatrixForLightSource(lightPosition, maxLightDistance);
        shadowAssemblyPipeline->getConfig().lightPosition = lightPosition;
    }
    void processMesh(Mesh* mesh, glm::mat4 viewMatrix, glm::vec3 cameraPosition){
        VkCommandBuffer cmd = shadowBufferPipeline->beginRender();
        shadowBufferPipeline->processMesh(mesh);
        depthBuffer->clearImage(0,0,0,0,cmd);
        depthBuffer->copyFromImage(shadowBufferPipeline->getDepthImages()[0], cmd);
        shadowBufferPipeline->endRender();
        shadowAssemblyPipeline->getConfig().normalMapEnabled = mesh->getMaterial()->getNormalMap()!=nullptr;
        shadowAssemblyPipeline->getConfig().previousAoEnabled = mesh->getMaterial()->getAoTexture()!=nullptr;
        shadowAssemblyPipeline->getWorldTransfomData().lightSpaceMatrix = shadowBufferPipeline->getViewData().lightSpaceMatrix;
        shadowAssemblyPipeline->getWorldTransfomData().viewMatrix = viewMatrix;
        shadowAssemblyPipeline->getWorldTransfomData().cameraPosition = cameraPosition;
        shadowAssemblyPipeline->getWorldTransfomData().worldMatrix = shadowBufferPipeline->getViewData().worldMatrix;
        if(shadowAssemblyPipeline->getConfig().normalMapEnabled or shadowAssemblyPipeline->getConfig().previousAoEnabled){
            shadowAssemblyPipeline->setNormalMap(mesh->getMaterial()->getNormalMap());
            shadowAssemblyPipeline->setPreviousAo(mesh->getMaterial()->getAoTexture());
            shadowAssemblyPipeline->updateSamplers();
        }
        shadowAssemblyPipeline->beginRender();
        shadowAssemblyPipeline->processMesh(mesh, false);
        shadowAssemblyPipeline->endRender();
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
        shadowAssemblyPipeline->setShadowMap(depthBuffer->getView());
        shadowAssemblyPipeline->updateSamplers();
    }
private:
    VkFormat findDepthFormat() {
        return device->findSupportedFormat(
                {VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT},
                VK_IMAGE_TILING_OPTIMAL,
                VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
    }
};
