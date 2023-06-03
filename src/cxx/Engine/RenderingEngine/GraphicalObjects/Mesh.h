//
// Created by Daniil on 28.03.2023.
//
#pragma once

#include "Vulkan/VulkanBuffers/VertexBuffer.h"
#include "Vulkan/VulkanBuffers/IndexBuffer.h"
#include "Vulkan/VulkanImage/VulkanImage.h"
#include "Material.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

struct MeshData {
    glm::vec3 position;
    glm::vec2 uv;
    glm::vec3 normal;

};

class Mesh
{
public:
static void sortMeshesByDistance(std::vector<Mesh*>& target, glm::vec3 cameraPosition){
    int changes = 1;
    while(changes>0){
        changes = 0;
        for(unsigned int i = 0; i<target.size()-1; i++){
            if(glm::length(cameraPosition-target[i]->position)<glm::length(cameraPosition- target[i+1]->position)){
                std::swap(target[i], target[i+1]);
                changes++;
            }
        }
    }
}
private:
    VertexBuffer *vBuffer;
    IndexBuffer *iBuffer;
    Material *material;
    std::string name;

    glm::vec3 position = glm::vec3(0);
    glm::vec3 rotation = glm::vec3(0);
    glm::vec3 scale = glm::vec3(1);
    glm::mat4 worldMatrix;

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

    void updateWorldMatrix(){
       worldMatrix = glm::mat4(1.0f);
       worldMatrix = glm::translate(worldMatrix, position);
       worldMatrix = glm::rotate(worldMatrix, rotation.x, glm::vec3(1, 0, 0));
       worldMatrix = glm::rotate(worldMatrix, rotation.y, glm::vec3(0, 1, 0));
       worldMatrix = glm::rotate(worldMatrix, rotation.z, glm::vec3(0, 0, 1));
       worldMatrix = glm::scale(worldMatrix, scale);
    }

    glm::mat4 getWorldMatrix() {
        return worldMatrix;
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
