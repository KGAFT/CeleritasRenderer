//
// Created by kgaft on 6/7/23.
//

#pragma once

#include "../RenderPipelineSecond.h"
#include "../GraphicalObjects/Material.h"
#include "../GraphicalObjects/Mesh.h"
#include <glm/glm.hpp>

namespace RenderEngine{
    struct GBufferConfig {
        alignas(4) int combinedMetallicRoughness;
        alignas(4) int opacityMapEnabled;
        alignas(4) int emissiveEnabled;
        alignas(4) int aoEnabled;
    };

    struct WorldTransformData{
        glm::mat4 viewMatrix;
        glm::mat4 worldMatrix;
        glm::vec3 cameraPosition;
    };

    class GBufferPipeline : public RenderPipeline{
    public:
        GBufferPipeline(VulkanDevice* device, int width, int height);
    private:
        WorldTransformData worldTransformData{};
        GBufferConfig gBufferConfig{};
        std::map<Material*, VulkanDescriptorSet*> materialDescriptors;
        VkCommandBuffer currentCmd;
    public:
        void registerMaterial(Material* material);
        void unRegisterMaterial(Material* material);
        void beginRender();
        void drawMesh(Mesh* mesh);
        void endRender();
        VulkanImage *getPositionsImage();
        VulkanImage *getAlbedoMapImage();
        VulkanImage *getNormalMapImage();
        VulkanImage *getMetallicRoughnessEmissiveInvao();
        VulkanImage* getAoImage();
    };
}




