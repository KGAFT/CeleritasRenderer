//
// Created by kgaft on 6/7/23.
//
#pragma once

#include "../../RenderPipeline.h"
#include "../../GraphicalObjects/Material.h"
#include "../../GraphicalObjects/Mesh.h"
#include <glm/glm.hpp>
#include "../../PrimitiveObjects/Quad.h"

namespace RenderEngine
{
    struct ShadowAssemblerConfig
    {
        alignas(16) glm::vec3 lightPosition;
        alignas(4) int normalMapEnabled;
        alignas(4) int previousAoEnabled;
    };

    struct LightWorldTransformData
    {
        glm::mat4 lightSpaceMatrix;
        glm::mat4 worldMatrix;
        glm::mat4 viewMatrix;
        glm::vec3 cameraPosition;
    };

    class DirectLightShadowAssemblyPipeline : public RenderPipeline
    {
        public:
            DirectLightShadowAssemblyPipeline(VulkanDevice* device, int width, int height);
        private:
            VulkanDevice* device;
            ShadowAssemblerConfig config{};
            LightWorldTransformData wtData{};
            VkCommandBuffer currentCmd;
            std::map<Material *, VulkanDescriptorSet *> materialDescriptors;
            VulkanImage* shadowMap;
        public:
            VkCommandBuffer beginRender();
            void processMesh(Mesh* mesh);
            void endRenderPass();
            void endRender();
            void registerMaterial(Material* material);
            void setShadowMap(VulkanImage* shadowMap);
            void unRegisterMeshMaterial(Material* material);
            ShadowAssemblerConfig& getConfig();
            LightWorldTransformData& getWorldTransformData();
            std::vector<VulkanImage*>& getOutputImages();
            void resize(int width, int height);

    };

}
