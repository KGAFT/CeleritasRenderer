//
// Created by Daniil on 05.02.2023.
//
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <string>
#include <iostream>
#include <vector>

#include "../Engine/GraphicalObjects/Mesh.h"
#include "AssimpGLMHelper.h"


using namespace std;


class ModelLoader {
private:
    vector<Mesh *> meshes;
    string directory;
    VulkanDevice *device;
    map<string, VulkanImage*> loadedTextures;

private:
    void processNode(aiNode *node, const aiScene *scene) {
        for (unsigned int i = 0; i < node->mNumMeshes; i++) {
            aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
            meshes.push_back(processMesh(mesh, scene));
        }
        // then do the same for each of its children
        for (unsigned int i = 0; i < node->mNumChildren; i++) {
            processNode(node->mChildren[i], scene);
        }
    }

    Mesh *processMesh(aiMesh *mesh, const aiScene *scene) {

        vector<unsigned int> indices;
        vector<MeshData> data;
        for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
            MeshData dataOfMesh{};

            dataOfMesh.position = glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
            dataOfMesh.normal = glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);

            if (mesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
            {
                dataOfMesh.uv = glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
            }
            data.push_back(dataOfMesh);
        }
        for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
            aiFace face = mesh->mFaces[i];
            for (unsigned int j = 0; j < face.mNumIndices; j++)
                indices.push_back(face.mIndices[j]);
        }


        VertexBuffer *vbo = new VertexBuffer(sizeof(float) * 8, data.size(), device, data.data());
        IndexBuffer *ibo = new IndexBuffer(device, indices.data(), indices.size());

        Mesh *currentMesh = new Mesh(vbo, ibo);

        loadTextures(scene, mesh, currentMesh);


        return currentMesh;
    }


    void loadTextures(const aiScene *scene, aiMesh *mesh, Mesh *currentMesh) {
        std::vector<Texture> textures;

        Material* material = new Material();
        VulkanImage *currentTexture = nullptr;
        aiString path;
        scene->mMaterials[mesh->mMaterialIndex]->GetTexture(aiTextureType_DIFFUSE, 0, &path, NULL, NULL, NULL, NULL,
                                                            NULL);


        currentTexture = tryGetLoadedMaterial(path.C_Str());
        if (currentTexture == nullptr) {
            loadTexture(path.C_Str());
        }
        currentTexture = tryGetLoadedMaterial(path.C_Str());

        if (currentTexture == nullptr) {
            scene->mMaterials[mesh->mMaterialIndex]->GetTexture(aiTextureType_BASE_COLOR, 0, &path, NULL, NULL, NULL,
                                                                NULL,
                                                                NULL);
            currentTexture = tryGetLoadedMaterial(path.C_Str());
            if (currentTexture == nullptr) {
                loadTexture(path.C_Str());
            }
        }
        textures.push_back({-2, ALBEDO_TEXTURE, currentTexture});

        scene->mMaterials[mesh->mMaterialIndex]->GetTexture(aiTextureType_NORMALS, 0, &path, NULL, NULL, NULL, NULL,
                                                            NULL);
        currentTexture = tryGetLoadedMaterial(path.C_Str());
        if (currentTexture == nullptr) {
            loadTexture(path.C_Str());
        }
        currentTexture = tryGetLoadedMaterial(path.C_Str());

        textures.push_back({-2, NORMAL_MAP_TEXTURE, currentTexture});

        scene->mMaterials[mesh->mMaterialIndex]->GetTexture(aiTextureType_EMISSIVE, 0, &path, NULL, NULL, NULL, NULL,
                                                            NULL);

        currentTexture = tryGetLoadedMaterial(path.C_Str());
        if (currentTexture == nullptr) {
            loadTexture(path.C_Str());
        }
        currentTexture = tryGetLoadedMaterial(path.C_Str());

        textures.push_back({-2, EMISSIVE_TEXTURE, currentTexture});
        scene->mMaterials[mesh->mMaterialIndex]->GetTexture(aiTextureType_AMBIENT_OCCLUSION, 0, &path, NULL, NULL, NULL,
                                                            NULL,
                                                            NULL);
        currentTexture = tryGetLoadedMaterial(path.C_Str());
        if (currentTexture == nullptr) {
            loadTexture(path.C_Str());
        }
        currentTexture = tryGetLoadedMaterial(path.C_Str());
        textures.push_back({-2, AO_TEXTURE, currentTexture});

        scene->mMaterials[mesh->mMaterialIndex]->GetTexture(aiTextureType_METALNESS, 0, &path, NULL, NULL, NULL, NULL,
                                                            NULL);
        currentTexture = tryGetLoadedMaterial(path.C_Str());
        if (currentTexture == nullptr) {
            loadTexture(path.C_Str());
        }
        currentTexture = tryGetLoadedMaterial(path.C_Str());
        textures.push_back({-2, METALLIC_ROUGHNESS_TEXTURE, currentTexture});


        scene->mMaterials[mesh->mMaterialIndex]->GetTexture(aiTextureType_OPACITY, 0, &path, NULL, NULL, NULL, NULL,
                                                            NULL);
        currentTexture = tryGetLoadedMaterial(path.C_Str());
        if (currentTexture == nullptr) {
            loadTexture(path.C_Str());
        }
        currentTexture = tryGetLoadedMaterial(path.C_Str());
        textures.push_back({-5, OPACITY_MAP_TEXTURE, currentTexture});
        material->populateTextures(textures);
        currentMesh->setMaterial(material);

    }
    int assimpTextureTypeToTextureType(aiTextureType textureType){
        switch(textureType){
            case aiTextureType_DIFFUSE|aiTextureType_BASE_COLOR:
                return ALBEDO_TEXTURE;
            case aiTextureType_NORMALS:
                return NORMAL_MAP_TEXTURE;
            case aiTextureType_EMISSIVE:
                return EMISSIVE_TEXTURE;
            case aiTextureType_AMBIENT_OCCLUSION:
                return AO_TEXTURE;
            case aiTextureType_METALNESS:
                return METALLIC_ROUGHNESS_TEXTURE;
            case aiTextureType_OPACITY:
                return OPACITY_MAP_TEXTURE;
            default:
                return -3;    
        }
    }
    VulkanImage *tryGetLoadedMaterial(const char *path) {

        for (const auto item: loadedTextures) {
            if (!item.first.compare(path)) {
                return item.second;
            }
        }
        return nullptr;
    }

    void loadTexture(const char *path) {
        try {
            string fullPath = (directory + '/' + path);
            VulkanImage* texture = VulkanImage::loadTexture(fullPath.c_str(), device);
            loadedTextures.insert(std::pair<string, VulkanImage *>(string(path), texture));

        } catch (std::runtime_error &ex) {

        }
    }

public:
    ModelLoader(VulkanDevice *device) : device(device) {}

    std::vector<Mesh *> loadModel(string path) {
        Assimp::Importer import;
        const aiScene *scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
            cout << "ERROR::ASSIMP::" << import.GetErrorString() << endl;
            return std::vector<Mesh *>();
        }
        directory = path.substr(0, path.find_last_of('/'));

        processNode(scene->mRootNode, scene);
        return meshes;
    }


    void clear() {
        meshes.clear();
        loadedTextures.clear();
        directory = "";
    }
};