//
// Created by Daniil on 02.04.2023.
//

#pragma once

#include "glad/glad.h"
#include "../../Util/IDestroyableObject.h"

class GLCElementBuffer : public IDestroyableObject{
public:
    friend class GLCVertexArrayObject;

    static GLCElementBuffer *createElementBufferObject(GLuint *indices, GLsizeiptr size) {
        GLuint eboId;
        glGenBuffers(1, &eboId);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, eboId);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, indices, GL_DYNAMIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        return new GLCElementBuffer(eboId, size / sizeof(GLuint));
    }

private:
    GLuint eboId;
    GLuint indicesAmount;
    bool atttachedToVao = false;

    GLCElementBuffer(GLuint eboId, GLuint indicesAmount) : eboId(eboId), indicesAmount(indicesAmount) {}

public:
    void destroy() override {
        IDestroyableObject::destroy();
        glDeleteBuffers(1, &eboId);
    }

public:
    void bind() const {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, eboId);
    }

    void unBind() const {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }

    [[nodiscard]] unsigned int getIndicesAmount() const {
        return indicesAmount;
    }


};