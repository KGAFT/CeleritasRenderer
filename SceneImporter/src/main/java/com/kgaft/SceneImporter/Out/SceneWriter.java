package com.kgaft.SceneImporter.Out;

import com.kgaft.SceneImporter.Data.Model.Model;
import com.kgaft.SceneImporter.Data.Scene;
import com.kgaft.SceneImporter.Out.TextureDataManager.TextureDataManager;
import com.kgaft.SceneImporter.Out.VerticesDataManager.VerticesDataManager;
import com.kgaft.SceneImporter.Data.Model.Mesh;
import org.joml.Vector3f;
import org.joml.Vector4f;

import java.io.File;
import java.io.FileWriter;
import java.io.IOException;

public class SceneWriter {
    private VerticesDataManager verticesDataManager = new VerticesDataManager();
    private TextureDataManager textureDataManager = new TextureDataManager();

    public SceneWriter() {
    }

    public void loadTextures(Scene scene) {
        scene.getModels().forEach(model -> {
            textureDataManager.parseTexturesFromModel(model);
        });
    }

    public void writeVerticesDataToFile(FileWriter writer) throws IOException {
        verticesDataManager.getVertexData().forEach(vertexData -> {
            try {
                writer.write("\nVERTICES ID: ");
                writer.write(vertexData.getId() + "\n");
                writer.write("POSITIONS: \n");
                for (float vertex : vertexData.getVertices()) {
                    writer.write("" + vertex + ";");
                }
                writer.write("\n");
                writer.write("NORMALS: \n");
                for (float normal : vertexData.getNormals()) {
                    writer.write("" + normal + ";");
                }
                writer.write("\n");
                writer.write("UV: \n");
                for (float uv : vertexData.getUvs()) {
                    writer.write("" + uv + ";");
                }
                writer.write("\n");
                writer.write("INDICES: \n");
                for (int index : vertexData.getIndices()) {
                    writer.write("" + index + ";");
                }
            } catch (IOException e) {
                e.printStackTrace();
            }
        });
    }

    public void writeSceneObjectsInfo(Scene scene, FileWriter writer) {
        scene.getModels().forEach(model -> {
            model.getMeshes().forEach(mesh -> {
                try {
                    writeMeshData(mesh, writer);
                } catch (IOException e) {
                    e.printStackTrace();
                }
            });
        });
    }

    private void writeMeshData(Mesh mesh, FileWriter writer) throws IOException {
        writer.write("\nMESH: \n");
        writer.write("NAME: " + mesh.getName() + "\n");
        writer.write("VERTICES_ID: " + mesh.getVerticesId() + "\n");
        writer.write("TEXTURES_IDS: ");
        mesh.getLoadedTextures().forEach(el -> {
            try {
                writer.write(el + ";");
            } catch (IOException e) {
                throw new RuntimeException(e);
            }
        });
        writer.write("\nCOLOR: ");
        writer.write(vectorToString(mesh.getBaseMeshColor())+"\n");
        writer.write("POSITION: ");
        writer.write(vectorToString(mesh.getPosition())+"\n");
        writer.write("POSITION_OFFSET: ");
        writer.write(vectorToString(mesh.getModelPositionOffset())+"\n");
        writer.write("ROTATION: ");
        writer.write(vectorToString(mesh.getRotation())+"\n");
        writer.write("SCALE: ");
        writer.write(vectorToString(mesh.getScale())+"\n");
    }
    private String vectorToString(Vector3f vector3f){
        return vector3f.x+" "+vector3f.y+" "+vector3f.z;
    }
    private String vectorToString(Vector4f vector4f){
        return vector4f.x+" "+vector4f.y+" "+vector4f.z+" "+vector4f.w;
    }

    public void writeTextures(FileWriter writer) throws IOException {
        textureDataManager.getLoadedTextures().values().forEach(texture -> {
            try {
                writer.write("\n" + "TEXTURE: \n");
                writer.write("ID: " + texture.getId() + " WIDTH: " + texture.getWidth() + " HEIGHT: " + texture.getHeight() + " CHANNELS: " + texture.getChannelsAmount() + " TYPE: " + texture.getType().toString());
                writer.write("\n");
                texture.getData().rewind();
                while (texture.getData().hasRemaining()) {
                    writer.write(texture.getData().get()+";");
                }
                writer.write("\n");
            } catch (IOException e) {
                e.printStackTrace();
            }

        });
    }

    public VerticesDataManager getVerticesDataManager() {
        return verticesDataManager;
    }

    public TextureDataManager getTextureDataManager() {
        return textureDataManager;
    }
}
