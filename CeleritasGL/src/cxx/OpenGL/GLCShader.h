//
// Created by Daniil on 02.04.2023.
//

#pragma once
#include <glad/glad.h>
#include "../Util/IDestroyableObject.h"
class GLCShader : public IDestroyableObject{
private:
    GLuint shaderId;
public:
    explicit GLCShader(GLuint shaderId) : shaderId(shaderId) {}
    void attach(){
        glUseProgram(shaderId);
    }

    void destroy() override {
        IDestroyableObject::destroy();
        glDeleteProgram(shaderId);
    }

    GLuint getShaderId() const {
        return shaderId;
    }
};