//
// Created by kgaft on 6/7/23.
//
#pragma once

#include "../RenderPipeline.h"
#include "../GraphicalObjects/Material.h"
#include "../GraphicalObjects/Mesh.h"
#include "GBufferPipeline.h"
#include <glm/glm.hpp>
#include "../PrimitiveObjects/Quad.h"
#include <Vulkan/VulkanImage/VulkanCubemapImage.h>
#define LIGHT_BLOCKS_AMOUNT 100

namespace RenderEngine{
    struct PointLight {
        alignas(16) glm::vec3 position;
        alignas(16) glm::vec3 color;
        alignas(4) float intensity;
    };

    struct DirectLight {
        alignas(16) glm::vec3 direction;
        alignas(16) glm::vec3 color;
        alignas(4) float intensity;
    };


    struct LightConfiguration {
        DirectLight directLights[LIGHT_BLOCKS_AMOUNT];
        PointLight pointLights[LIGHT_BLOCKS_AMOUNT];
        alignas(4) int enabledDirects = 0;
        alignas(4) int enabledPoints = 0;


        alignas(4) float emissiveIntensity = 2;
        alignas(4) float emissiveShininess = 1;
        alignas(4) float gammaCorrect = 1.0f / 2.2f;
        alignas(4) float ambientIntensity = 0.03f;
    };

    struct VertexConfig {
        glm::vec3 cameraPosition = glm::vec3(0);
    };

    class GameAssemblyPipeline : public RenderPipeline {
    public:
        GameAssemblyPipeline(VulkanDevice* device, int width, int height);
    private:
        VulkanDevice* device;
        VulkanDescriptorSet* gBufferDescriptor;
        LightConfiguration lightConfig;
        VertexConfig vertexConfig;
        Mesh* quadDrawMesh;
    public:
        void setGBuffer(GBufferPipeline* gBufferPipeline);
        void setAo(VulkanImage* ao);
        void setBackground(VulkanImage* background);
        void setReflectionImage(VulkanCubemapImage* reflection);
        LightConfiguration &getLightConfig();
        VertexConfig &getVertexConfig();
        void update();
        VulkanImage* getOutput();
        void confirmInputs();
    };
}




