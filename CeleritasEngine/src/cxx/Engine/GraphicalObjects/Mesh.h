//
// Created by Daniil on 28.03.2023.
//
#pragma once

#include "../../Vulkan/VulkanBuffers/VertexBuffer.h"
#include "../../Vulkan/VulkanBuffers/IndexBuffer.h"
#include "../../Vulkan/VulkanImage/VulkanImage.h"
#include <glm/vec3.hpp>

class Mesh{
private:
    VertexBuffer* vBuffer;
    IndexBuffer* iBuffer;
    std::vector<VulkanImage*> textures;
    glm::vec3 position = glm::vec3(0);
    glm::vec3 rotation = glm::vec3(0);
    glm::vec3 scale = glm::vec3(1);
public:
    Mesh(VertexBuffer *vBuffer, IndexBuffer *iBuffer) : vBuffer(vBuffer), iBuffer(iBuffer) {}

    std::vector<VulkanImage *> &getTextures()  {
        return textures;
    }

    void setTextures(const std::vector<VulkanImage *> &textures) {
        Mesh::textures = textures;
    }

    glm::vec3 &getPosition()  {
        return position;
    }

    void setPosition(glm::vec3 position) {
        Mesh::position = position;
    }

     glm::vec3 &getRotation()  {
        return rotation;
    }

    void setRotation(glm::vec3 rotation) {
        Mesh::rotation = rotation;
    }

    glm::vec3 &getScale(){
        return scale;
    }

    void setScale(const glm::vec3 &scale) {
        Mesh::scale = scale;
    }
    ~Mesh(){
        delete vBuffer;
        delete iBuffer;
    }
};
