package com.kgaft.SceneImporter.Out.VerticesDataManager;

public class VertexData {
    private float[] vertices;
    private float[] uvs;
    private float[] normals;
    private int[] indices;
    private int id;

    public VertexData(float[] vertices, float[] uvs, float[] normals, int[] indices) {
        this.vertices = vertices;
        this.uvs = uvs;
        this.normals = normals;
        this.indices = indices;
    }

    public float[] getVertices() {
        return vertices;
    }

    public void setVertices(float[] vertices) {
        this.vertices = vertices;
    }

    public float[] getUvs() {
        return uvs;
    }

    public void setUvs(float[] uvs) {
        this.uvs = uvs;
    }

    public float[] getNormals() {
        return normals;
    }

    public void setNormals(float[] normals) {
        this.normals = normals;
    }

    public int[] getIndices() {
        return indices;
    }

    public void setIndices(int[] indices) {
        this.indices = indices;
    }

    public int getId() {
        return id;
    }

    protected void setId(int id) {
        this.id = id;
    }
}
