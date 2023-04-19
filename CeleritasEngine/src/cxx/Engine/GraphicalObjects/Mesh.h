//
// Created by Daniil on 28.03.2023.
//
#pragma once

#include <Vulkan/VulkanBuffers/VertexBuffer.h>
#include <Vulkan/VulkanBuffers/IndexBuffer.h>
#include <Vulkan/VulkanImage/VulkanImage.h>
#include "Material.h"
#include <glm/glm.hpp>

struct MeshData {
    glm::vec3 position;
    glm::vec2 uv;
    glm::vec3 normal;

};

class Mesh
{
private:
    VertexBuffer *vBuffer;
    IndexBuffer *iBuffer;
    Material *material;
    std::string name;

    glm::vec3 position = glm::vec3(0);
    glm::vec3 rotation = glm::vec3(0);
    glm::vec3 scale = glm::vec3(1);

public:
    Mesh(VertexBuffer *vBuffer, IndexBuffer *iBuffer) : vBuffer(vBuffer), iBuffer(iBuffer) {}

    glm::vec3 &getPosition()
    {
        return position;
    }

    std::string getName()
    {
        return this->name;
    }


    void setName(std::string name)
    {
        this->name = name;
    }

    void setPosition(glm::vec3 position)
    {
        Mesh::position = position;
    }

    glm::vec3 &getRotation()
    {
        return rotation;
    }

    VertexBuffer *getVBuffer()
    {
        return vBuffer;
    }

    IndexBuffer *getIBuffer()
    {
        return iBuffer;
    }
    void draw(VkCommandBuffer commandBuffer){
        vBuffer->bind(commandBuffer);
        if(iBuffer!=nullptr){
            iBuffer->bind(commandBuffer);
            iBuffer->draw(commandBuffer);
            return;

        }
        vBuffer->draw(commandBuffer);
        
    }
    Material *getMaterial()
    {
        return material;
    }

    void setRotation(glm::vec3 rotation)
    {
        Mesh::rotation = rotation;
    }

    glm::vec3 &getScale()
    {
        return scale;
    }


    void setMaterial(Material *material)
    {
        this->material = material;
    }

    void setScale(const glm::vec3 &scale)
    {
        Mesh::scale = scale;
    }
    ~Mesh()
    {
        delete vBuffer;
        delete iBuffer;
    }
};
