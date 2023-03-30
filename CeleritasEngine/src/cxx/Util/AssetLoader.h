#pragma once

#include <fstream>
#include <vector>
#include <string>
#include "../Vulkan/VulkanBuffers/VertexBuffer.h"
#include "../Vulkan/VulkanBuffers/IndexBuffer.h"
#include "../Vulkan/VulkanImage/VulkanImage.h"
#include "StringUtil.h"
#include "../Engine/GraphicalObjects/Mesh.h"
#include "../Engine/GraphicalObjects/Material.h"

struct Vertices
{
    int id = 0;

    VertexBuffer *vertexBuffer;
    IndexBuffer *indexBuffer;
};

class AssetLoader
{
private:
    std::vector<Texture> textures;
    std::vector<Vertices> vertices;

public:
    /**
     * Call to this method after calling loadTextures and loadVertices
    */

    void loadMeshes(const char *pathToFile, std::vector<Mesh*>& out)
    {
        std::fstream file(pathToFile);
        Mesh* currentMesh = parseMesh(file);
        while(currentMesh!=nullptr){
            out.push_back(currentMesh);
            currentMesh = parseMesh(file);
        }
    }

    void loadTextures(const char *pathToFile, VulkanDevice *device)
    {
        std::fstream fileStream(pathToFile);
        Texture tempRes = loadTexture(device, fileStream);
        while (tempRes.id != -5)
        {
            textures.push_back(tempRes);
            tempRes = loadTexture(device, fileStream);
        }
        fileStream.close();
    }
    void loadVertices(const char *pathToFile, VulkanDevice *device)
    {
        std::fstream file(pathToFile);
        Vertices curVertices = loadVertexBuffer(device, file);
        while (curVertices.id != -5)
        {
            vertices.push_back(curVertices);
            curVertices = loadVertexBuffer(device, file);
        }
        file.close();
    }

private:
    Mesh *assembleMesh(std::string meshName, int verticesId, std::vector<int> &texturesIds, glm::vec4 baseMeshColor, glm::vec3 position, glm::vec3 rotation, glm::vec3 scale)
    {
        Vertices meshVertices{-5, nullptr, nullptr};
        for (auto el : vertices)
        {
            if (el.id == verticesId)
            {
                meshVertices = el;
                break;
            }
        }
        if (meshVertices.id == -5)
        {
            throw std::runtime_error("Failed to load vertices for id: " + verticesId);
        }
        std::vector<Texture> meshTextures;
        for (const auto &index : texturesIds)
        {
            for (const auto &element : textures)
            {
                if (element.id == index)
                {
                    meshTextures.push_back(element);
                    break;
                }
            }
        }
        Material *material = new Material();
        material->populateTextures(meshTextures);
        Mesh *result = new Mesh(meshVertices.vertexBuffer, meshVertices.indexBuffer);
        result->setMaterial(material);
        result->setPosition(position);
        result->setRotation(rotation);
        result->setScale(scale);
        result->setName(meshName);
        return result;
    }

    Mesh *parseMesh(std::fstream &file)
    {
        using namespace std;
        bool found = false;
        std::string curLine;
        while (getline(file, curLine))
        {
            if (!strcmp(curLine.c_str(), "MESH: "))
            {
                found = true;
                break;
            }
        }
        if (!found)
        {
            return nullptr;
        }
        vector<string> lineArgs;
        getline(file, curLine);
        StringUtil::split(curLine, lineArgs, ' ');
        string name = lineArgs[1];
        getline(file, curLine);

        StringUtil::split(curLine, lineArgs, ' ');
        int verticesId = stoi(lineArgs[1]);
        vector<int> texturesIds;
        getline(file, curLine);

        StringUtil::split(curLine, lineArgs, ' ');
        string texturesId = lineArgs[1];

        StringUtil::split(texturesId, lineArgs, ';');
        for (const auto &item : lineArgs)
        {
            texturesIds.push_back(stoi(item));
        }
        getline(file, curLine);

        StringUtil::split(curLine, lineArgs, ':');
        glm::vec4 baseMeshColor = StringUtil::stringToVector(lineArgs[1]);
        getline(file, curLine);

        StringUtil::split(curLine, lineArgs, ':');
        glm::vec3 position = StringUtil::stringToVector3(lineArgs[1]);

        getline(file, curLine);
        StringUtil::split(curLine, lineArgs, ':');
        position += StringUtil::stringToVector3(lineArgs[1]);
        getline(file, curLine);
        StringUtil::split(curLine, lineArgs, ':');
        glm::vec3 rotation = StringUtil::stringToVector3(lineArgs[1]);
        getline(file, curLine);
        StringUtil::split(curLine, lineArgs, ':');
        glm::vec3 scale = StringUtil::stringToVector3(lineArgs[1]);

        Mesh* res = assembleMesh(name, verticesId, texturesIds, baseMeshColor, position, rotation, scale);
        return res;
    }

    Texture loadTexture(VulkanDevice *device, std::fstream &file)
    {
        using namespace std;
        string curLine;
        bool found = false;
        while (getline(file, curLine))
        {
            if (!std::strcmp(curLine.c_str(), "TEXTURE: "))
            {
                found = true;
                break;
            }
        }
        getline(file, curLine);
        vector<string> lineArgs;
        if (found)
        {
            StringUtil::split(curLine, lineArgs, ' ');
            if (lineArgs.size() >= 8)
            {
                int id = stoi(lineArgs[1]);
                int width = stoi(lineArgs[3]);
                int height = stoi(lineArgs[5]);
                int channels = stoi(lineArgs[7]);
                std::vector<char> textureData;
                getline(file, curLine);
                string curByte;
                for (char el : curLine)
                {
                    if (el == ';')
                    {
                        try
                        {
                            textureData.push_back((char)stoi(curByte));
                            curByte = "";
                        }
                        catch (std::exception &e)
                        {
                            std::cerr << e.what() << std::endl;
                        }
                    }
                    else
                    {
                        curByte += el;
                    }
                }
                VulkanImage *image = VulkanImage::loadBinTexture(device, textureData.data(), width, height, channels);
                Texture res {id, stringToTextureType(lineArgs[lineArgs.size() - 1].c_str()), image};
                return res;
            }
        }
        return {-5, -5, nullptr};
    }

    Vertices loadVertexBuffer(VulkanDevice *device, std::fstream &file)
    {
        using namespace std;
        string curLine;
        vector<string> lineArgs;
        bool found = false;
        while (getline(file, curLine))
        {
            StringUtil::split(curLine, lineArgs, ' ');
            if (curLine.size() > 0 and !std::strcmp(lineArgs[0].c_str(), "VERTICES"))
            {
                found = true;
                break;
            }
            lineArgs.clear();
        }
        if (found)
        {
            int verticesID = stoi(lineArgs[2]);
            std::vector<float> positions;
            std::vector<float> normals;
            std::vector<float> uvs;
            std::vector<unsigned int> indices;
            populateVertices(positions, normals, uvs, indices, file);
            std::vector<float> vBufferData;
            for (int i = 0; i < positions.size() / 3; i++)
            {
                vBufferData.push_back(positions[i * 3]);
                vBufferData.push_back(positions[i * 3 + 1]);
                vBufferData.push_back(positions[i * 3 + 2]);
                vBufferData.push_back(normals[i * 3]);
                vBufferData.push_back(normals[i * 3 + 1]);
                vBufferData.push_back(normals[i * 3 + 2]);
                vBufferData.push_back(uvs[i * 2]);
                vBufferData.push_back(uvs[i * 2 + 1]);
            }
            VertexBuffer *vBuffer = new VertexBuffer(8 * sizeof(float), positions.size() / 3, device, vBufferData.data());
            IndexBuffer *iBuffer = new IndexBuffer(device, indices.data(), indices.size());
            return {verticesID, vBuffer, iBuffer};
        }
        return {-5, nullptr, nullptr};
    }

    void populateVertices(std::vector<float> &positions,
                          std::vector<float> &normals,
                          std::vector<float> &uvs,
                          std::vector<unsigned int> &indices, std::fstream &file)
    {
        std::string curLine;
        std::vector<std::string> lineArgs;
        getline(file, curLine);
        if (!std::strcmp(curLine.c_str(), "POSITIONS: "))
        {
            getline(file, curLine);

            StringUtil::split(curLine, lineArgs, ';');
            for (const auto &item : lineArgs)
            {
                positions.push_back(stof(item));
            }
        }
        getline(file, curLine);
        if (!std::strcmp(curLine.c_str(), "NORMALS: "))
        {
            getline(file, curLine);

            StringUtil::split(curLine, lineArgs, ';');
            for (const auto &item : lineArgs)
            {
                normals.push_back(stof(item));
            }
        }
        getline(file, curLine);
        if (!std::strcmp(curLine.c_str(), "UV: "))
        {
            getline(file, curLine);

            StringUtil::split(curLine, lineArgs, ';');
            for (const auto &item : lineArgs)
            {
                uvs.push_back(stof(item));
            }
        }
        getline(file, curLine);
        if (!std::strcmp(curLine.c_str(), "INDICES: "))
        {
            getline(file, curLine);

            StringUtil::split(curLine, lineArgs, ';');
            for (const auto &item : lineArgs)
            {
                indices.push_back(stoi(item));
            }
        }
    }

    int stringToTextureType(const char *source)
    {
        if (!std::strcmp(source, "ALBEDO_TEXTURE"))
        {
            return ALBEDO_TEXTURE;
        }
        if (!std::strcmp(source, "DIFFUSE_TEXTURE"))
        {
            return ALBEDO_TEXTURE;
        }
        if (!std::strcmp(source, "METALLIC_TEXTURE"))
        {
            return METALLIC_TEXTURE;
        }
        if (!std::strcmp(source, "METALLIC_ROUGHNESS_TEXTURE"))
        {
            return METALLIC_ROUGHNESS_TEXTURE;
        }
        if (!std::strcmp(source, "ROUGHNESS_TEXTURE"))
        {
            return ROUGHNESS_TEXTURE;
        }
        if (!std::strcmp(source, "NORMAL_MAP_TEXTURE"))
        {
            return NORMAL_MAP_TEXTURE;
        }
        if (!std::strcmp(source, "EMISSIVE_TEXTURE"))
        {
            return EMISSIVE_TEXTURE;
        }
        if (!std::strcmp(source, "AO_TEXTURE"))
        {
            return AO_TEXTURE;
        }
        if (!std::strcmp(source, "OPACITY_MAP_TEXTURE"))
        {
            return OPACITY_MAP_TEXTURE;
        }
        return -1;
    }
};