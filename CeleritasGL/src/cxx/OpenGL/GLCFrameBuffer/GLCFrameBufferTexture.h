//
// Created by Daniil on 02.04.2023.
//

#pragma once

#include "../../Util/IDestroyableObject.h"
#include <glad/glad.h>

class GLCFrameBufferTexture : public IDestroyableObject{
private:
    GLuint textureId;
public:
    GLCFrameBufferTexture(unsigned int width, unsigned int height){
        glGenTextures(1, &textureId);
        glBindTexture(GL_TEXTURE_2D, textureId);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    void recreate(unsigned int width, unsigned int height){
        destroy();
        destroyed = false;
        glGenTextures(1, &textureId);
        glBindTexture(GL_TEXTURE_2D, textureId);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    void bind(){
        glBindTexture(GL_TEXTURE_2D, textureId);
    }

    void unBind(){
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    void destroy() override {
        IDestroyableObject::destroy();
        glDeleteTextures(1, &textureId);
    }

    GLuint getTextureId() const {
        return textureId;
    }
};