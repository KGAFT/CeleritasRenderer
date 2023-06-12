//
// Created by kgaft on 6/7/23.
//
#pragma once


#include "../../RenderPipeline.h"
#include "../../GraphicalObjects/Material.h"
#include "../../GraphicalObjects/Mesh.h"
#include <glm/glm.hpp>
#include "../../PrimitiveObjects/Quad.h"

namespace RenderEngine{
    struct LightViewData {
        glm::mat4 lightSpaceMatrix;
        glm::mat4 worldMatrix;
    };

    class DirectLightShadowBuffer : public RenderPipeline{
    public:
        DirectLightShadowBuffer(VulkanDevice* device, int width, int height);
    private:
        VulkanDevice* device;
        LightViewData viewData{};
        VkCommandBuffer currentCmd;
    public:
        void recalculateMatrixForLightSource(glm::vec3 lightPos, int maxLightShadowDistance);
        void beginRender();
        void processMesh(Mesh* mesh);
        void endRender();
    };
}





