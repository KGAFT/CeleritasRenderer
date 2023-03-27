package com.kgaft.SceneImporter.Out.VerticesDataManager;

import java.util.ArrayList;
import java.util.List;

public class VerticesDataManager {
    private List<VertexData> vertexData = new ArrayList<>();
    private int vertexIdCounter = 0;
    public int registerVertices(float[] positions, float[] normals, float[] uvs, int[] indices){
        VertexData data = new VertexData(positions, uvs, normals, indices);
        data.setId(vertexIdCounter);
        vertexIdCounter++;
        vertexData.add(data);
        return vertexIdCounter-1;
    }

    public List<VertexData> getVertexData() {
        return vertexData;
    }
    public VertexData getVertexData(int vertexId){
        for (VertexData vertexDatum : vertexData) {
            if(vertexDatum.getId()==vertexId){
                return vertexDatum;
            }
        }
        return null;
    }
}
