#include "SkyboxPipeline.h"

using namespace RenderEngine::SkyboxPipeline;

SkyboxPipeline(VulkanDevice* device, int width, int height) : RenderPipeline(device, nullptr), device(device){
    RenderPipelineBuilder builder;
    builder
}