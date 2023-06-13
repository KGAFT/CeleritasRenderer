#pragma once

#include "../RenderPipeline.h"
#include "../GraphicalObjects/Material.h"
#include "../GraphicalObjects/Mesh.h"
#include "../PrimitiveObjects/Skybox.h"
#include <Vulkan/VulkanImage/VulkanCubemapImage.h>
#include <glm/glm.hpp>
namespace RenderEngine
{
    struct SkyboxConfig
    {
        glm::mat4 cameraMatrix;
    };
    class SkyboxPipeline : public RenderPipeline
    {
    public:
        SkyboxPipeline(VulkanDevice *device, int width, int height);

    private:
        VulkanDevice *device;
        VulkanDescriptorSet *descriptorSet;
        SkyboxConfig skyboxConfig{};
        Mesh* skyboxMesh;
    public:
        void update(glm::mat4 cameraMatrix);
        void setSkyboxImage(VulkanCubemapImage* skyboxImage);
    };

}
