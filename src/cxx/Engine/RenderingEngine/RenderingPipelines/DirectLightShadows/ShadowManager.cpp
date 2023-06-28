#include "ShadowManager.h"

using namespace RenderEngine;

ShadowManager::ShadowManager(VulkanDevice *device, int bufferQuadLen, int windowWidth, int windowHeight) : device(device)
{
    shadowBufferPipeline = new DirectLightShadowBuffer(device, bufferQuadLen, bufferQuadLen);
    depthBuffer = VulkanImage::createImageWithFormat(device, bufferQuadLen, bufferQuadLen, findDepthFormat());
    shadowAssemblyPipeline = new DirectLightShadowAssemblyPipeline(device, windowWidth, windowHeight);
    shadowAssemblyPipeline->setShadowMap(depthBuffer);
}

void ShadowManager::setupLightView(glm::vec3 lightPosition, float maxLightDistance)
{
    shadowBufferPipeline->recalculateMatrixForLightSource(lightPosition, maxLightDistance);
    shadowAssemblyPipeline->getConfig().lightPosition = lightPosition;
}

void ShadowManager::beginShadowPass()
{
    shadowCmd = shadowBufferPipeline->beginRender();
}
void ShadowManager::processMesh(Mesh *mesh)
{
    shadowBufferPipeline->processMesh(mesh);
    meshesBuffer.push_back(mesh);
}

void ShadowManager::registerMesh(Mesh *mesh)
{
    shadowAssemblyPipeline->registerMaterial(mesh->getMaterial());
}

void ShadowManager::unRegisterMesh(Mesh *mesh)
{
    shadowAssemblyPipeline->unRegisterMeshMaterial(mesh->getMaterial());
}

void ShadowManager::endShadowPass(glm::mat4 viewMatrix, glm::vec3 cameraPosition)
{

    shadowBufferPipeline->endRenderPass();
    depthBuffer->copyFromImage(shadowBufferPipeline->getDepthImages()[0], VK_IMAGE_LAYOUT_GENERAL, shadowCmd, true);
    shadowBufferPipeline->endRender();
    shadowAssemblyPipeline->beginRender();
    for (const auto &item : meshesBuffer)
    {
        shadowAssemblyPipeline->getConfig().normalMapEnabled = item->getMaterial()->getNormalMap() != nullptr;
        shadowAssemblyPipeline->getConfig().previousAoEnabled = item->getMaterial()->getAoTexture() != nullptr;
        shadowAssemblyPipeline->getWorldTransformData().lightSpaceMatrix = shadowBufferPipeline->getViewData().lightSpaceMatrix;
        shadowAssemblyPipeline->getWorldTransformData().viewMatrix = viewMatrix;
        shadowAssemblyPipeline->getWorldTransformData().cameraPosition = cameraPosition;

        shadowAssemblyPipeline->processMesh(item);
    }
    shadowAssemblyPipeline->endRenderPass();
    shadowAssemblyPipeline->endRender();
    meshesBuffer.clear();
}

VulkanImage *ShadowManager::getOutput()
{
    return shadowAssemblyPipeline->getOutputImages()[0];
}

void ShadowManager::resizeOutput(int width, int height)
{
    shadowAssemblyPipeline->resize(width, height);
}

void ShadowManager::resizeInternal(int squareLen)
{
    shadowBufferPipeline->resize(squareLen, squareLen);
    depthBuffer->resize(squareLen, squareLen);
    shadowAssemblyPipeline->setShadowMap(depthBuffer);
}

VkFormat ShadowManager::findDepthFormat()
{
    return device->findSupportedFormat(
        {VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT},
        VK_IMAGE_TILING_OPTIMAL,
        VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
}