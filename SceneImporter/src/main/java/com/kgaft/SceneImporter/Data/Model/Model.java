package com.kgaft.SceneImporter.Data.Model;

import org.joml.Vector3f;

import java.util.ArrayList;
import java.util.List;

public class Model {
    private List<Mesh> meshes = new ArrayList<>();

    public Model(List<Mesh> meshes) {
        this.meshes = meshes;
    }

    public List<Mesh> getMeshes() {
        return meshes;
    }

    public void setMeshes(List<Mesh> meshes) {
        this.meshes = meshes;
    }
    public void setPosition(Vector3f position){
        meshes.forEach(mesh -> mesh.setPosition(position));
    }
    public void setRotation(Vector3f rotation){
        meshes.forEach(mesh -> mesh.getRotation().add(rotation));
    }
    public void setScale(Vector3f scale){
        meshes.forEach(mesh -> mesh.getScale().add(scale));
    }
}
