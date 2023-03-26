package com.kgaft.SceneImporter.Data.Model;

import com.kgaft.SceneImporter.Data.Texture.Texture;
import org.joml.Vector3f;
import org.joml.Vector4f;

import java.util.ArrayList;
import java.util.List;

public class Mesh {
    private String name;
    private int verticesId;
    private Vector4f baseMeshColor = new Vector4f(1, 1, 1, 1);
    private Vector3f position = new Vector3f(0);
    private Vector3f modelPositionOffset = new Vector3f(0);
    private Vector3f rotation = new Vector3f();
    private Vector3f scale = new Vector3f(1);
    private List<Texture> textures = new ArrayList<>();
    private List<Integer> loadedTextures = new ArrayList<>();

    public Mesh(String name, int verticesId) {
        this.name = name;
        this.verticesId = verticesId;
    }

    public String getName() {
        return name;
    }

    public void setName(String name) {
        this.name = name;
    }

    public int getVerticesId() {
        return verticesId;
    }

    public void setVerticesId(int verticesId) {
        this.verticesId = verticesId;
    }

    public Vector4f getBaseMeshColor() {
        return baseMeshColor;
    }

    public void setBaseMeshColor(Vector4f baseMeshColor) {
        this.baseMeshColor = baseMeshColor;
    }

    public List<Texture> getTextures() {
        return textures;
    }

    public void setTextures(List<Texture> textures) {
        this.textures = textures;
    }

    public Vector3f getModelPositionOffset() {
        return modelPositionOffset;
    }

    public void setModelPositionOffset(Vector3f modelPositionOffset) {
        this.modelPositionOffset = modelPositionOffset;
    }

    public Vector3f getPosition() {
        return position;
    }

    public void setPosition(Vector3f position) {
        this.position = position;
    }

    public List<Integer> getLoadedTextures() {
        return loadedTextures;
    }

    public void setLoadedTextures(List<Integer> loadedTextures) {
        this.loadedTextures = loadedTextures;
    }

    public Vector3f getRotation() {
        return rotation;
    }

    public void setRotation(Vector3f rotation) {
        this.rotation = rotation;
    }

    public Vector3f getScale() {
        return scale;
    }

    public void setScale(Vector3f scale) {
        this.scale = scale;
    }
}
