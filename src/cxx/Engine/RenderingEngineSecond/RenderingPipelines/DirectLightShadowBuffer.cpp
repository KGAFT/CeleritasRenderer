//
// Created by kgaft on 6/7/23.
//

#include "DirectLightShadowBuffer.h"

RenderEngine::DirectLightShadowBuffer::DirectLightShadowBuffer(VulkanDevice *device, int width, int height) : RenderPipelineSecond(device, nullptr), device(device){
    RenderPipelineBuilder builder;
    builder.setPathToShader("shaders/DirectLightShadowBuffer")
            ->setStartFramebufferWidth(width)
            ->setStartFramebufferHeight(height)
            ->addVertexInput(0, 3, sizeof(float), VK_FORMAT_R32G32B32_SFLOAT)
            ->addVertexInput(0, 2, sizeof(float), VK_FORMAT_R32G32_SFLOAT)
            ->addVertexInput(0, 3, sizeof(float), VK_FORMAT_R32G32B32_SFLOAT)
            ->addPushConstant(sizeof(LightViewData), VK_SHADER_STAGE_VERTEX_BIT)
            ->setImageRenderOutputAmount(1);
    RenderPipelineSecond::initialize(builder);
    RenderPipelineSecond::getPushConstants()[0]->setData(&viewData);
}

void
RenderEngine::DirectLightShadowBuffer::recalculateMatrixForLightSource(glm::vec3 lightPos, int maxLightShadowDistance) {
    glm::mat4 lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 0.1f, (float) maxLightShadowDistance);
    glm::mat4 lightView = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
    viewData.lightSpaceMatrix = lightProjection * lightView;
}

void RenderEngine::DirectLightShadowBuffer::beginRender() {
    currentCmd = RenderPipelineSecond::beginRender();
}

void RenderEngine::DirectLightShadowBuffer::processMesh(Mesh *mesh) {
    viewData.worldMatrix = mesh->getWorldMatrix();
    RenderPipelineSecond::getPushConstants()[0]->setData(&viewData);
    RenderPipelineSecond::updatePushConstants();
    mesh->draw(currentCmd);
}

void RenderEngine::DirectLightShadowBuffer::endRender() {
    RenderPipelineSecond::endRenderPass();
    RenderPipelineSecond::endRender();
}
