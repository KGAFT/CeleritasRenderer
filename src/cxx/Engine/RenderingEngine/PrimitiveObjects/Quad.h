//
// Created by Daniil on 10.04.2023.
//
#pragma once

#include "../GraphicalObjects/Mesh.h"

float QuadVertices[]{
        -1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
        -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
        1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
        1.0f, -1.0f, 0.0f, 1.0f, 0.0f};

unsigned int QuadIndices[]{
        0, 1, 2,
        3, 2, 1};

Mesh* quadMesh = nullptr;

Mesh* acquireQuadMesh(VulkanDevice* device){
    if(quadMesh==nullptr){
        quadMesh = new Mesh(new VertexBuffer(5, 4, device, QuadVertices), new IndexBuffer(device, QuadIndices, 6));
    }
    return quadMesh;
}