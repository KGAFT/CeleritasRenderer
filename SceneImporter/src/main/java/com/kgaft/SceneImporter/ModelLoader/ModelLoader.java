package com.kgaft.SceneImporter.ModelLoader;

import com.kgaft.SceneImporter.Data.Model.Mesh;
import com.kgaft.SceneImporter.Data.Model.Model;
import com.kgaft.SceneImporter.Data.Texture.Texture;
import com.kgaft.SceneImporter.Data.Texture.TextureType;
import com.kgaft.SceneImporter.Out.VerticesDataManager.VerticesDataManager;
import org.lwjgl.PointerBuffer;
import org.lwjgl.assimp.*;

import java.nio.IntBuffer;
import java.util.ArrayList;
import java.util.List;

public class ModelLoader {
    private String workDirectory = "";
    private VerticesDataManager verticesDataManager;

    public ModelLoader(VerticesDataManager verticesDataManager) {
        this.verticesDataManager = verticesDataManager;
    }

    public Model loadModel(String modelPath) {
        AIScene scene = Assimp.aiImportFile(modelPath, Assimp.aiProcess_FlipUVs | Assimp.aiProcess_Triangulate);
        System.out.println(Assimp.aiGetErrorString());
        workDirectory = modelPath.substring(0, modelPath.lastIndexOf("/"));
        return processScene(scene);
    }

    public VerticesDataManager getVerticesDataManager() {
        return verticesDataManager;
    }

    public void setVerticesDataManager(VerticesDataManager verticesDataManager) {
        this.verticesDataManager = verticesDataManager;
    }

    private Model processScene(AIScene scene) {
        PointerBuffer meshes = scene.mMeshes();
        List<Mesh> result = new ArrayList<>();
        for (int c = 0; c < scene.mNumMeshes(); c++) {
            AIMesh mesh = AIMesh.create(meshes.get(c));
            result.add(processMesh(mesh, scene));
        }
        Model model = new Model(result);
        return model;
    }

    private Mesh processMesh(AIMesh mesh, AIScene scene) {

        AIVector3D.Buffer verticesBuf = mesh.mVertices();
        AIVector3D.Buffer normalsBuf = mesh.mNormals();
        AIVector3D.Buffer uvsBuffer = mesh.mTextureCoords(0);
        AIFace.Buffer buffer = mesh.mFaces();

        int indicesCapacity = 0;
        while (buffer.remaining() > 0) {
            indicesCapacity += buffer.get().mIndices().remaining();
        }
        buffer.rewind();
        float[] positions = new float[verticesBuf.remaining() * 3];
        float[] normals = new float[normalsBuf.remaining() * 3];
        float[] uvs = new float[uvsBuffer.remaining() * 2];
        int[] indices = new int[indicesCapacity];
        int counter = 0;
        while (verticesBuf.remaining() > 0) {
            AIVector3D vector3D = verticesBuf.get();
            positions[counter] = vector3D.x();
            positions[counter + 1] = vector3D.y();
            positions[counter + 2] = vector3D.z();
            counter += 3;
        }
        counter = 0;
        while (normalsBuf.remaining() > 0) {
            AIVector3D normal = normalsBuf.get();
            normals[counter] = normal.x();
            normals[counter + 1] = normal.y();
            normals[counter + 2] = normal.z();
            counter += 3;
        }
        counter = 0;
        while (uvsBuffer.remaining() > 0) {
            AIVector3D vector3D = uvsBuffer.get();
            uvs[counter] = vector3D.x();
            uvs[counter + 1] = vector3D.y();
            counter += 2;
        }


        counter = 0;
        while (buffer.remaining() > 0) {
            IntBuffer index = buffer.get().mIndices();
            while (index.remaining() > 0) {
                indices[counter] = index.get();
                counter++;
            }
        }

        Mesh currentMesh = new Mesh(mesh.mName().dataString(), verticesDataManager.registerVertices(positions, normals, uvs, indices));

        currentMesh.setName(mesh.mName().dataString());
        currentMesh.setTextures(tryParseTextures(mesh, scene, workDirectory));
        return currentMesh;
    }

    private List<Texture> tryParseTextures(AIMesh mesh, AIScene scene, String workDirectory) {
        List<Texture> result = new ArrayList<>();
        AIMaterial material = AIMaterial.create(scene.mMaterials().get(mesh.mMaterialIndex()));
        AIString currentPath = AIString.calloc();
        Assimp.aiGetMaterialTexture(material, Assimp.aiTextureType_DIFFUSE, 0, currentPath, (IntBuffer) null, null, null, null, null, null);
        if(currentPath.dataString().length()>0){
            Texture diffuse = new Texture(workDirectory+"/"+currentPath.dataString(), TextureType.DIFFUSE_TEXTURE);
            result.add(diffuse);
        }
        Assimp.aiGetMaterialTexture(material, Assimp.aiTextureType_BASE_COLOR, 0, currentPath, (IntBuffer) null, null, null, null, null, null);
        if(currentPath.dataString().length()>0){
            Texture albedo = new Texture(workDirectory+"/"+currentPath.dataString(), TextureType.ALBEDO_TEXTURE);
            result.add(albedo);
        }
        Assimp.aiGetMaterialTexture(material, Assimp.aiTextureType_NORMALS, 0, currentPath, (IntBuffer) null, null, null, null, null, null);
        if(currentPath.dataString().length()>0){
            Texture normals = new Texture(workDirectory+"/"+currentPath.dataString(), TextureType.NORMAL_MAP_TEXTURE);
            result.add(normals);
        }



        Assimp.aiGetMaterialTexture(material, Assimp.aiTextureType_DIFFUSE_ROUGHNESS, 0, currentPath, (IntBuffer) null, null, null, null, null, null);
        String roughnessTex = currentPath.dataString();
        Assimp.aiGetMaterialTexture(material, Assimp.aiTextureType_METALNESS, 0, currentPath, (IntBuffer) null, null, null, null, null, null);
        String metalnessTexture = currentPath.dataString();
        if(roughnessTex.equals(metalnessTexture) || roughnessTex.length()==0){
            Texture metallicRoughness = new Texture(workDirectory+"/"+metalnessTexture, TextureType.METALLIC_ROUGHNESS_TEXTURE);
            result.add(metallicRoughness);
        }
        else{
            result.add(new Texture(workDirectory+"/"+metalnessTexture, TextureType.METALLIC_TEXTURE));
            result.add(new Texture(workDirectory+"/"+roughnessTex, TextureType.ROUGHNESS_TEXTURE));
        }

        Assimp.aiGetMaterialTexture(material, Assimp.aiTextureType_AMBIENT_OCCLUSION, 0, currentPath, (IntBuffer) null, null, null, null, null, null);
        if(currentPath.dataString().length()>0){
            Texture ao = new Texture(workDirectory+"/"+currentPath.dataString(), TextureType.AO_TEXTURE);
            result.add(ao);
        }


        Assimp.aiGetMaterialTexture(material, Assimp.aiTextureType_EMISSIVE, 0, currentPath, (IntBuffer) null, null, null, null, null, null);
        if(currentPath.dataString().length()>0){
            Texture emissive = new Texture(workDirectory+"/"+currentPath.dataString(), TextureType.EMISSIVE_TEXTURE);
            result.add(emissive);
        }

        Assimp.aiGetMaterialTexture(material, Assimp.aiTextureType_OPACITY, 0, currentPath, (IntBuffer) null, null, null, null, null, null);
        if(currentPath.dataString().length()>0){
            Texture opacity = new Texture(workDirectory+"/"+currentPath.dataString(), TextureType.OPACITY_MAP_TEXTURE);
            result.add(opacity);
        }
        return result;
    }
}
