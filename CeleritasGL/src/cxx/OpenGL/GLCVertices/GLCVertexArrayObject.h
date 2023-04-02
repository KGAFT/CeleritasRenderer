//
// Created by Daniil on 02.04.2023.
//
#pragma once

#include "glad/glad.h"
#include <vector>
#include <stdexcept>
#include "../../Util/IDestroyableObject.h"
#include "GLCVertexBuffer.h"
#include "GLCElementBuffer.h"

class GLCVertexArrayObject : public IDestroyableObject {
private:
    unsigned int vaoId;
    std::vector<std::pair<GLCVertexBuffer *, unsigned int>> vbos;
    GLCElementBuffer *elementBufferObject = nullptr;
public:
    GLCVertexArrayObject() {
        glGenVertexArrays(1, &vaoId);
    }

    void attachBuffer(GLCVertexBuffer *bufferObject, GLuint layoutPosition) {
        if (!bufferObject->attached) {
            bind();
            bufferObject->bind();

            glEnableVertexAttribArray(layoutPosition);
            glVertexAttribPointer(layoutPosition, bufferObject->getVertexCoordAmount(), bufferObject->getType(),
                                  GL_FALSE,
                                  0, (void *) nullptr);

            unBind();
            bufferObject->unBind();
            vbos.push_back(std::pair(bufferObject, layoutPosition));
            bufferObject->attached = true;
        } else {
            throw std::runtime_error("Error vbo allready attached to another vao");
        }

    }

    void attachElementBuffer(GLCElementBuffer *ebo) {
        if (!ebo->atttachedToVao) {
            bind();
            ebo->bind();
            unBind();
            ebo->unBind();
            this->elementBufferObject = ebo;
            ebo->atttachedToVao = true;
        } else {
            throw std::runtime_error("ElementBufferObject can be used only in the current vertex array");
        }
    }

    void draw() {
        if (elementBufferObject != nullptr) {
            for (const auto &item: vbos){
                glEnableVertexAttribArray(item.second);
            }
            bind();
            glDrawElements(GL_TRIANGLES, elementBufferObject->getIndicesAmount(), GL_UNSIGNED_INT, nullptr);
            unBind();
        } else {
            throw std::runtime_error("Error: ebo is not attached");
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
        for (unsigned int counter = 0; counter < vbos.size(); counter++) {
            delete vbos[counter].first;
        }
    }
};