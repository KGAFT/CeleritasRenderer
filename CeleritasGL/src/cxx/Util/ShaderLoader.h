//
// Created by Daniil on 02.04.2023.
//
#pragma once
#include "ShaderConfParser.h"
#include "../OpenGL/GLCShader.h"
#include <glad/glad.h>
#include <stdexcept>
#include <iostream>
class ShaderLoader{
public:
    static GLCShader* loadShaderFromDirectory(const char* targetDirectory){
        std::vector<ShaderInfo> shadersInfo;
        ShaderConfParser::parseShader(targetDirectory, shadersInfo);
        std::vector<GLuint> shaderSubPrograms;
        for (const auto &item: shadersInfo){
            shaderSubPrograms.push_back(loadShaderFromFile(item.path.c_str(), shaderTypeToGL(item.type)));
        }
        GLuint programId = glCreateProgram();
        for (const auto item: shaderSubPrograms){
            glAttachShader(programId, item);
        }
        glLinkProgram(programId);
        for (const auto &item: shaderSubPrograms){
            glDeleteShader(item);
        }
        int success;
        char infoLog[512];
        glGetProgramiv(programId, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(programId, 512, NULL, infoLog);
            std::cout << infoLog << std::endl;
            throw std::runtime_error(infoLog);
        }
        return new GLCShader(programId);
    }
private:
    static GLenum shaderTypeToGL(int shaderType){
        switch(shaderType){
            case VERTEX_SHADER:
                return GL_VERTEX_SHADER;
            case FRAGMENT_SHADER:
                return GL_FRAGMENT_SHADER;
            case GEOMETRY_SHADER:
                return GL_GEOMETRY_SHADER;
            default:
                return -1;
        }
    }

    static GLuint loadShaderFromFile(const char* fileName, GLenum type) {
        std::string shaderSource;
        getFileTextContent(fileName, shaderSource);
        const char *shaderSourceCode = shaderSource.c_str();
        GLuint shaderId = glCreateShader(type);
        glShaderSource(shaderId, 1, &shaderSourceCode, NULL);
        glCompileShader(shaderId);
        int success;
        char infoLog[512];
        glGetShaderiv(shaderId, GL_COMPILE_STATUS, &success);

        if (!success) {
            glGetShaderInfoLog(shaderId, 512, NULL, infoLog);
            std::cout << infoLog << std::endl;
            throw std::runtime_error(infoLog);
        }
        return shaderId;
    }
    static void getFileTextContent(const char *filePathWithName, std::string& content) {
        std::ifstream fileReader(filePathWithName, std::ios::binary);
        if (fileReader) {

            fileReader.seekg(0, std::ios::end);
            content.resize(fileReader.tellg());
            fileReader.seekg(0, std::ios::beg);
            fileReader.read(&content[0], content.size());
            fileReader.close();
            return;
        }
        throw std::runtime_error("Failed to open file");

    }
};