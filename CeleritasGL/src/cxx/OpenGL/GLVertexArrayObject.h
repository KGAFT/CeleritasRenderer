//
// Created by Daniil on 02.04.2023.
//
#pragma once

#include <glad/glad.h>
#include <vector>
#include <stdexcept>
#include "../Util/IDestroyableObject.h"
#include "GLVertexBuffer.h"

class GLVertexArrayObject : public IDestroyableObject{
private:
    unsigned int vaoId;
    std::vector<GLVertexBuffer*> vbos;
public:
    GLVertexArrayObject() {
        glGenVertexArrays(1, &vaoId);
    }

    void attachBuffer(GLVertexBuffer *bufferObject, GLuint layoutPosition) {
        if (!bufferObject->attached) {
            bind();
            bufferObject->bind();

            glEnableVertexAttribArray(layoutPosition);
            glVertexAttribPointer(layoutPosition, bufferObject->getVertexCoordAmount(), bufferObject->getType(),
                                  GL_FALSE,
                                  bufferObject->getTypeSize() * bufferObject->getVertexCoordAmount(), (void *) nullptr);

            unBind();
            bufferObject->unBind();
            vbos.push_back(bufferObject);
            bufferObject->attached = true;
        } else {
            throw std::runtime_error("Error vbo allready attached to another vao");
        }

    }

    void bind() const {
        glBindVertexArray(vaoId);
    }

    void unBind() {
        glBindVertexArray(0);
    }

    void destroy() override {
        IDestroyableObject::destroy();
        for(unsigned int counter = 0; counter<vbos.size(); counter++){
            delete vbos[counter];
        }
    }
};