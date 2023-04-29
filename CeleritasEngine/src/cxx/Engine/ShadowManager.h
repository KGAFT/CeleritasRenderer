#pragma once
#include "Pipelines/ShadowBufferPipeline.h"
#include "Pipelines/ShadowAssemblyPipeline.h"
class ShadowManager{
private:
        ShadowBufferPipeline* shadowBufferPipeline;
        ShadowAssemblyPipeline* shadowAssemblyPipeline;
        VulkanDevice* device;
        VulkanImage* depthBuffer;
        VkCommandBuffer shadowCmd;
        std::vector<Mesh*> meshesBuffer;
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

    void beginShadowPass(){
        shadowCmd = shadowBufferPipeline->beginRender();
    }

    void processMesh(Mesh* mesh){
        shadowBufferPipeline->processMesh(mesh);
        meshesBuffer.push_back(mesh);
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
            shadowAssemblyPipeline->getWorldTransfomData().worldMatrix = shadowBufferPipeline->getViewData().worldMatrix;
            if(shadowAssemblyPipeline->getConfig().normalMapEnabled or shadowAssemblyPipeline->getConfig().previousAoEnabled){
                shadowAssemblyPipeline->setNormalMap(item->getMaterial()->getNormalMap());
                shadowAssemblyPipeline->setPreviousAo(item->getMaterial()->getAoTexture());
                shadowAssemblyPipeline->updateSamplers();
            }

            shadowAssemblyPipeline->processMesh(item, false);

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
