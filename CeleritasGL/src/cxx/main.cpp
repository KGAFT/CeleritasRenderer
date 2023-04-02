#include <iostream>
#include "Window/Window.h"
#include "OpenGL/GLCVertices/GLCVertexArrayObject.h"
#include "OpenGL/GLCVertices/GLCElementBuffer.h"
#include "Util/ShaderLoader.h"
#include "OpenGL/GLCFrameBuffer/GLCFrameBuffer.h"

int main() {
    Window::createWindow(800, 600, "Hello world", nullptr);
    glViewport(0, 0, 800, 600);

    GLCVertexArrayObject vao;
    float vertices[]{
            -1.0f,  1.0f, 0.0f,
            -1.0f, -1.0f, 0.0f,
            1.0f,  1.0f, 0.0f,
            1.0f, -1.0f, 0.0f,
    };
    float uv[]{
            0.0f, 1.0f,
            0.0f, 0.0f,
            1.0f, 1.0f,
            1.0f, 0.0f,
    };

    unsigned int indices[]{
            0, 1, 2,
            3, 2, 1
    };

    float secondVertices[]{
        0,0,0.7f,
        0.5f,0,0.7f,
        0,0.5f,0.7f,


        0.5f, 0,0.5f,
        0,0,0.5f,
        0.5f,0.5f,0.5f
    };
    float color []{
            0,0,0, 1,
            0.5f,0,0, 1,
            0,0.5f,0, 1,

            0,0.2,-0.5f, 1.0f,
            0.5f,0.2,-0.5f, 1.0f,
            0,0.7f,-0.5f, 1.0f
    };
    unsigned int secondIndices[]{
            0, 1, 2,
            3,4,5
    };
    GLCVertexBuffer* verticesVbo = GLCVertexBuffer::createVertexBufferObject(vertices, 4*3*sizeof(float), 3, GL_FLOAT, sizeof(float));
    GLCVertexBuffer* uvsVbo = GLCVertexBuffer::createVertexBufferObject(uv, 4*2*sizeof(float), 2, GL_FLOAT, sizeof(float));
    GLCElementBuffer* ebo = GLCElementBuffer::createElementBufferObject(indices, 6*sizeof(unsigned int));
    vao.attachBuffer(verticesVbo, 0);
    vao.attachBuffer(uvsVbo, 1);
    vao.attachElementBuffer(ebo);
    GLCShader* assemblyShader = ShaderLoader::loadShaderFromDirectory("shaders/AssemblyTest");

    GLCShader* deferredShader = ShaderLoader::loadShaderFromDirectory("shaders/DeferredTest");

    GLCFrameBuffer* frameBuffer = new GLCFrameBuffer(2, 800, 600);

    GLCVertexArrayObject secondVao;
    GLCVertexBuffer* secondVerticesBO = GLCVertexBuffer::createVertexBufferObject(secondVertices, 3*6*sizeof(float), 3, GL_FLOAT, sizeof(float));
    GLCVertexBuffer* colorBO = GLCVertexBuffer::createVertexBufferObject(color, 4*6*sizeof(float), 4, GL_FLOAT, sizeof(float));
    GLCElementBuffer* secondEbo = GLCElementBuffer::createElementBufferObject(secondIndices, 6*sizeof(unsigned int));
    secondVao.attachBuffer(secondVerticesBO, 0);
    secondVao.attachBuffer(colorBO, 1);
    secondVao.attachElementBuffer(secondEbo);

    while(!Window::getInstance()->needToClose()){
        deferredShader->attach();
        frameBuffer->bind();
        secondVao.draw();
        frameBuffer->unBind();

        glViewport(0,0,800,600);
        glEnable(GL_DEPTH_TEST);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(0.5f,0.2f,0.5f,1.0f);
        assemblyShader->attach();
        glActiveTexture(GL_TEXTURE0);
        frameBuffer->getTextures()[0]->bind();
        GLuint texLoc = glGetUniformLocation(assemblyShader->getShaderId(), "firstColorSampler");
        glUniform1i(texLoc, 0);
        vao.draw();
        Window::getInstance()->postRenderEvents();
    }
    return 0;
}
