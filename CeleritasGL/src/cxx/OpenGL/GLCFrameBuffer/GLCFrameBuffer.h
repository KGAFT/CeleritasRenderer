//
// Created by Daniil on 02.04.2023.
//

#pragma once

#include <vector>
#include <stdexcept>
#include "GLCFrameBufferTexture.h"
#include "GLCDepthTexture.h"
#include "../../Util/IDestroyableObject.h"
class GLCFrameBuffer : public IDestroyableObject{
private:
    unsigned int frameBufferId;
    std::vector<GLCFrameBufferTexture*> textures;
    GLCDepthTexture* depthTexture;
    unsigned int renderBufferDepth;
    unsigned int width, height;
public:
    GLCFrameBuffer(unsigned int frameBufferTexturesAmount, unsigned int width, unsigned int height) : width(width), height(height){
        glGenFramebuffers(1, &frameBufferId);
        glBindFramebuffer(GL_FRAMEBUFFER, frameBufferId);
        createTextures(frameBufferTexturesAmount, width, height);
    }
    void createTextures(unsigned int frameBufferTexturesAmount, unsigned int width, unsigned int height){
        std::vector<unsigned int> attachments;
        for (int i = 0; i < frameBufferTexturesAmount; ++i){
            GLCFrameBufferTexture* texture = new GLCFrameBufferTexture(width, height);
            texture->bind();
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0+i, GL_TEXTURE_2D, texture->getTextureId(), 0);
            texture->unBind();
            attachments.push_back(GL_COLOR_ATTACHMENT0+i);
            textures.push_back(texture);
        }
        depthTexture = new GLCDepthTexture(width, height);
        depthTexture->bind();
        //!!!!
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTexture->getTextureId(), 0);

        glDrawBuffers(attachments.size(), attachments.data());
        glReadBuffer(GL_NONE);
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            throw std::runtime_error("Frame buffer is not complete!");
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
    void recreate(unsigned int width, unsigned int height){
        std::vector<unsigned int> attachments;
        glDeleteFramebuffers(1, &frameBufferId);
        glDeleteRenderbuffers(1, &renderBufferDepth);
        glGenFramebuffers(1, &frameBufferId);
        glBindFramebuffer(GL_FRAMEBUFFER, frameBufferId);
        unsigned int counter = 0;
        for (const auto &item: textures){
            item->recreate(width, height);
            item->bind();
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0+counter, GL_TEXTURE_2D, item->getTextureId(), 0);
            item->unBind();
            attachments.push_back(GL_COLOR_ATTACHMENT0+counter);
            counter++;
        }
        depthTexture->recreate(width, height);
        depthTexture->bind();
        //!!!!
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTexture->getTextureId(), 0);
        glDrawBuffers(attachments.size(), attachments.data());
        glReadBuffer(GL_NONE);
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            throw std::runtime_error("Frame buffer is not complete!");
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        this->width = width;
        this->height = height;

    }
    void bind(){
        glEnable(GL_DEPTH_TEST);
        glViewport(0, 0, width, height);
        glBindFramebuffer(GL_FRAMEBUFFER, frameBufferId);
        glClear(GL_DEPTH_BUFFER_BIT|GL_COLOR_BUFFER_BIT);
    }
    void unBind(){
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    const std::vector<GLCFrameBufferTexture *> &getTextures() const {
        return textures;
    }

    GLCDepthTexture *getDepthTexture() const {
        return depthTexture;
    }

    void destroy() override {
        IDestroyableObject::destroy();
        glDeleteFramebuffers(1, &frameBufferId);
        glDeleteRenderbuffers(1, &renderBufferDepth);
        for (const auto &item: textures){
            delete item;
        }
    }
};