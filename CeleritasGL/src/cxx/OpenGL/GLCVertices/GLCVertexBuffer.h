//
// Created by Daniil on 02.04.2023.
//

#pragma once

#include "glad/glad.h"
#include "../../Util/IDestroyableObject.h"

class GLCVertexBuffer : public IDestroyableObject{
public:
    friend class GLCVertexArrayObject;

    static GLCVertexBuffer *
    createVertexBufferObject(void *vertices, GLsizeiptr verticesSize, unsigned int vertexCoordsAmount, GLenum type,
                             size_t typeSize) {
        GLuint vboId;
        glGenBuffers(1, &vboId);
        glBindBuffer(GL_ARRAY_BUFFER, vboId);
        glBufferData(GL_ARRAY_BUFFER, verticesSize, vertices, GL_DYNAMIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        return new GLCVertexBuffer(vboId, vertexCoordsAmount, type, typeSize);
    }

private:
    GLuint vboId;
    uint16_t vertexCoordAmount;
    GLenum type;
    size_t typeSize;
    bool attached = false;
    GLCVertexBuffer(GLuint vboId, uint16_t vertexCoordAmount, GLenum type, size_t typeSize) : vboId(vboId),
                                                                                              vertexCoordAmount(
            vertexCoordAmount),
                                                                                              type(type),
                                                                                              typeSize(typeSize) {}

public:
    void bind() const {
        glBindBuffer(GL_ARRAY_BUFFER, vboId);
    }

    void unBind() {
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }


    [[nodiscard]] unsigned int getVertexCoordAmount() const {
        return vertexCoordAmount;
    }

    [[nodiscard]] GLenum getType() const {
        return type;
    }

    [[nodiscard]] size_t getTypeSize() const {
        return typeSize;
    }

    void destroy() override {
        IDestroyableObject::destroy();
        glDeleteBuffers(1, &vboId);
    }

};