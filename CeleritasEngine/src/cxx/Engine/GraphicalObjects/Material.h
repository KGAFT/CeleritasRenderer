#pragma once

#include <Vulkan/VulkanImage/VulkanImage.h>

#define ALBEDO_TEXTURE 0
#define METALLIC_TEXTURE 1
#define METALLIC_ROUGHNESS_TEXTURE 2
#define ROUGHNESS_TEXTURE 3
#define NORMAL_MAP_TEXTURE 4
#define EMISSIVE_TEXTURE 5
#define AO_TEXTURE 6
#define OPACITY_MAP_TEXTURE 7

struct Texture
{
    int id;
    int type;
    VulkanImage *image;
};


class Material
{
private:
    VulkanImage *albedoTexture = nullptr;
    VulkanImage *normalMap = nullptr;
    VulkanImage *metallicTexture = nullptr;
    VulkanImage *roughnessTexture = nullptr;
    VulkanImage *metallicRoughnessTexture = nullptr;
    VulkanImage *aoTexture = nullptr;
    VulkanImage *emissiveTexture = nullptr;
    VulkanImage *opacityMapTexture = nullptr;

public:

    void populateTextures(std::vector<Texture>& materials){
        for(const auto& el : materials){
            switch(el.type){
                case ALBEDO_TEXTURE:
                    this->albedoTexture = el.image;
                    break;
                case METALLIC_TEXTURE:
                    this->metallicTexture = el.image;
                    break;
                case METALLIC_ROUGHNESS_TEXTURE:
                    this->metallicRoughnessTexture = el.image;
                    break;
                case ROUGHNESS_TEXTURE:
                    this->roughnessTexture = el.image;
                    break;
                case NORMAL_MAP_TEXTURE:
                    this->normalMap = el.image;
                    break;
                case EMISSIVE_TEXTURE:
                    this->emissiveTexture = el.image;
                    break;
                case AO_TEXTURE:
                    this->aoTexture = el.image;
                    break;
                case OPACITY_MAP_TEXTURE:
                    this->opacityMapTexture = el.image;
                    break;
                default:
                    break;    
            }
        }
    }


    void setAlbedoTexture(VulkanImage *albedoTexture)
    {
        this->albedoTexture = albedoTexture;
    }

    void setNormalMap(VulkanImage *normalMap)
    {
        this->normalMap = normalMap;
    }

    void setMetallicTexture(VulkanImage *metallicTexture)
    {
        this->metallicTexture = metallicTexture;
    }

    void setRoughnessTexture(VulkanImage *roughnessTexture)
    {
        this->roughnessTexture = roughnessTexture;
    }

    void setMetallicRoughnessTexture(VulkanImage *metallicRoughnessTexture)
    {
        this->metallicRoughnessTexture = metallicRoughnessTexture;
    }

    void setAoTexture(VulkanImage *aoTexture)
    {
        this->aoTexture = aoTexture;
    }

    void setEmissiveTexture(VulkanImage *emissiveTexture)
    {
        this->emissiveTexture = emissiveTexture;
    }

    void setOpacityMapTexture(VulkanImage *opacityMapTexture)
    {
        this->opacityMapTexture = opacityMapTexture;
    }

    VulkanImage *getAlbedoTexture()
    {
        return albedoTexture;
    }

    VulkanImage *getNormalMap()
    {
        return normalMap;
    }

    VulkanImage *getMetallicTexture()
    {
        return metallicTexture;
    }

    VulkanImage *getRoughnessTexture()
    {
        return roughnessTexture;
    }

    VulkanImage *getMetallicRoughnessTexture()
    {
        return metallicRoughnessTexture;
    }

    VulkanImage *getAoTexture()
    {
        return aoTexture;
    }

    VulkanImage *getEmissiveTexture()
    {
        return emissiveTexture;
    }

    VulkanImage *getOpacityMapTexture()
    {
        return opacityMapTexture;
    }
};