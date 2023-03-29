#pragma once


#include <fstream>
#include <vector>
#include <string>
#include "../Vulkan/VulkanBuffers/VertexBuffer.h"
#include "../Vulkan/VulkanBuffers/IndexBuffer.h"
#include "../Vulkan/VulkanImage/VulkanImage.h"
#include "StringUtil.h"
#include "../Engine/GraphicalObjects/Mesh.h"

#define ALBEDO_TEXTURE 0
#define METALLIC_TEXTURE 1
#define METALLIC_ROUGHNESS_TEXTURE 2
#define ROUGHNESS_TEXTURE 3
#define NORMAL_MAP_TEXTURE 4
#define EMISSIVE_TEXTURE 5
#define AO_TEXTURE 6
#define OPACITY_MAP_TEXTURE 7

struct Texture {
    int id;
    int type;
    VulkanImage *image;
};

struct Vertices {
    int id = 0;

    VertexBuffer *vertexBuffer;
    IndexBuffer *indexBuffer;
};

class AssetLoader {
private:
    std::vector<Texture> textures;
    std::vector<Vertices> vertices;
public:

    Mesh* parseMesh(std::fstream& file){
        using namespace std;
        bool found = false;
        std::string curLine;
        while(getline(file, curLine)){
            if(!strcmp(curLine.c_str(), "MESH: ")){
                found = true;
                break;
            }
        }
        if(!found){
            return nullptr;
        }
        vector<string>lineArgs;
        getline(file, curLine);
        StringUtil::split(curLine, lineArgs, ' ');
        string name = lineArgs[1];
    }

    void loadTextures(const char *pathToFile, VulkanDevice *device) {
        std::fstream fileStream(pathToFile);
        Texture tempRes = loadTexture(device, fileStream);
        while (tempRes.id != -5) {
            textures.push_back(tempRes);
            tempRes = loadTexture(device, fileStream);

        }
        fileStream.close();
    }
    void loadVertices(const char* pathToFile, VulkanDevice* device){
        std::fstream file(pathToFile);
        Vertices curVertices = loadVertexBuffer(device, file);
        while(curVertices.id!=-5){
            vertices.push_back(curVertices);
            curVertices = loadVertexBuffer(device, file);
        }
        file.close();
    }
private:
    Texture loadTexture(VulkanDevice *device, std::fstream &file) {
        using namespace std;
        string curLine;
        bool found = false;
        while (getline(file, curLine)){
            if(!std::strcmp(curLine.c_str(), "TEXTURE: ")){
                found = true;
            }
        }
        getline(file, curLine);
        vector<string> stringArgs;
        if(found) {
            StringUtil::split(curLine, stringArgs, ' ');
            if(stringArgs.size()>=8){
                int id = stoi(stringArgs[1]);
                int width = stoi(stringArgs[3]);
                int height = stoi(stringArgs[5]);
                int channels = stoi(stringArgs[7]);
                std::vector<char> textureData;
                getline(file, curLine);
                string curByte;
                for (char el: curLine) {
                    if (el == ';') {
                        try {
                            textureData.push_back((char) stoi(curByte));
                            curByte = "";
                        } catch (std::exception &e) {
                            std::cerr << e.what() << std::endl;
                        }

                    } else {
                        curByte += el;
                    }
                }
                VulkanImage *image = VulkanImage::loadBinTexture(device, textureData.data(), width, height, channels);

                return {id, stringToTextureType(stringArgs[stringArgs.size()-1].c_str()), image};
            }
        }
        return {-5, -5, nullptr};
    }

    Vertices loadVertexBuffer(VulkanDevice *device, std::fstream &file) {
        using namespace std;
        string curLine;
        vector<string> lineArgs;
        bool found = false;
        while (getline(file, curLine)) {
            StringUtil::split(curLine, lineArgs, ' ');
            if (curLine.size()>0 and !std::strcmp(lineArgs[0].c_str(), "VERTICES")) {
                found = true;
                break;
            }
            lineArgs.clear();
        }
        if(found){
            int verticesID = stoi(lineArgs[2]);
            std::vector<float> positions;
            std::vector<float> normals;
            std::vector<float> uvs;
            std::vector<unsigned int> indices;
            populateVertices(positions, normals, uvs, indices, file);
            std::vector<float> vBufferData;
            for(int i = 0; i<positions.size()/3; i++){
                vBufferData.push_back(positions[i*3]);
                vBufferData.push_back(positions[i*3+1]);
                vBufferData.push_back(positions[i*3+2]);
                vBufferData.push_back(normals[i*3]);
                vBufferData.push_back(normals[i*3+1]);
                vBufferData.push_back(normals[i*3+2]);
                vBufferData.push_back(uvs[i*2]);
                vBufferData.push_back(uvs[i*2+1]);
            }
            VertexBuffer* vBuffer = new VertexBuffer(8*sizeof(float), positions.size()/3, device, vBufferData.data());
            IndexBuffer* iBuffer = new IndexBuffer(device, indices.data(), indices.size());
            return {verticesID, vBuffer, iBuffer};
        }
       return {-5, nullptr, nullptr};
    }

    void populateVertices(std::vector<float> &positions,
                          std::vector<float> &normals,
                          std::vector<float> &uvs,
                          std::vector<unsigned int> &indices, std::fstream& file) {
        std::string curLine;
        std::vector<std::string> lineArgs;
        getline(file, curLine);
        if (!std::strcmp(curLine.c_str(), "POSITIONS: ")) {
            getline(file, curLine);
            lineArgs.clear();
            StringUtil::split(curLine, lineArgs, ';');
            for (const auto &item: lineArgs) {
                positions.push_back(stof(item));
            }
        }
        getline(file, curLine);
        if (!std::strcmp(curLine.c_str(), "NORMALS: ")) {
            getline(file, curLine);
            lineArgs.clear();
            StringUtil::split(curLine, lineArgs, ';');
            for (const auto &item: lineArgs) {
                normals.push_back(stof(item));
            }
        }
        getline(file, curLine);
        if (!std::strcmp(curLine.c_str(), "UV: ")) {
            getline(file, curLine);
            lineArgs.clear();
            StringUtil::split(curLine, lineArgs, ';');
            for (const auto &item: lineArgs) {
                uvs.push_back(stof(item));
            }
        }
        getline(file, curLine);
        if (!std::strcmp(curLine.c_str(), "INDICES: ")) {
            getline(file, curLine);
            lineArgs.clear();
            StringUtil::split(curLine, lineArgs, ';');
            for (const auto &item: lineArgs) {
                indices.push_back(stoi(item));
            }
        }
        lineArgs.clear();
    }

    int stringToTextureType(const char *source) {
        if (!std::strcmp(source, "ALBEDO_TEXTURE")) {
            return ALBEDO_TEXTURE;
        }
        if (!std::strcmp(source, "DIFFUSE_TEXTURE")) {
            return ALBEDO_TEXTURE;
        }
        if (!std::strcmp(source, "METALLIC_TEXTURE")) {
            return METALLIC_TEXTURE;
        }
        if (!std::strcmp(source, "METALLIC_ROUGHNESS_TEXTURE")) {
            return METALLIC_ROUGHNESS_TEXTURE;
        }
        if (!std::strcmp(source, "ROUGHNESS_TEXTURE")) {
            return ROUGHNESS_TEXTURE;
        }
        if (!std::strcmp(source, "NORMAL_MAP_TEXTURE")) {
            return NORMAL_MAP_TEXTURE;
        }
        if (!std::strcmp(source, "EMISSIVE_TEXTURE")) {
            return EMISSIVE_TEXTURE;
        }
        if (!std::strcmp(source, "AO_TEXTURE")) {
            return AO_TEXTURE;
        }
        if (!std::strcmp(source, "OPACITY_MAP_TEXTURE")) {
            return OPACITY_MAP_TEXTURE;
        }
        return -1;
    }
};