#pragma once

#include "../RenderPipelineSecond.h"
#include "../GraphicalObjects/Material.h"
#include "../GraphicalObjects/Mesh.h"
#include <Vulkan/VulkanImage/VulkanCubemapImage.h>
#include <glm/glm.hpp>
namespace RenderEngine
{
    struct SkyboxConfig
    {
        mat4 cameraMatrix;
    };
    class SkyboxPipeline : public RenderPipeline
    {
    public:
        SkyboxPipeline(VulkanDevice *device, int width, int height);

    private:
        VulkanDevice *device;
        VulkanDescriptorSet *descriptorSet;
        SkyboxConfig skyboxConfig{};
    public:
        void update();
        void setSkyboxImage(VulkanCubemapImage* skyboxImage);
    };

}
