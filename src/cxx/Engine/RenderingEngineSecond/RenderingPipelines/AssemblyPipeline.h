//
// Created by kgaft on 6/7/23.
//

#pragma once

#include "../RenderPipelineSecond.h"
#include "Vulkan/VulkanBuffers/VertexBuffer.h"
#include "Vulkan/VulkanBuffers/IndexBuffer.h"
#include "../../RenderingEngine/GraphicalObjects/Mesh.h"
#include "../../RenderingEngine/PrimitiveObjects/Quad.h"
namespace RenderEngine{
    struct AssemblyPipelineSetup{
        glm::vec4 colorAmplifier = glm::vec4(1.0f);
        glm::vec4 colorAdder = glm::vec4(0.0f);
    };

    class AssemblyPipeline : public RenderPipelineSecond{
    public:
        AssemblyPipeline(VulkanDevice* device, VulkanSwapChain* swapChain, int width, int height);
    private:
        VulkanDevice* device;
        VulkanSwapChain* swapChain;
        VulkanDescriptorSet* inputsDescriptorSet;
        Mesh* quadDrawMesh;
        AssemblyPipelineSetup setup;
    public:
        void setGamePlaceHolder(VulkanImage* placeHolder);
        void update();
    };

}

