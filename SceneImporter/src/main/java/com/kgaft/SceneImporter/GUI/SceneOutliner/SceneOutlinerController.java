package com.kgaft.SceneImporter.GUI.SceneOutliner;

import com.kgaft.SceneImporter.Data.Model.Mesh;
import com.kgaft.SceneImporter.Data.Model.Model;
import com.kgaft.SceneImporter.Data.Scene;
import com.kgaft.SceneImporter.Out.VerticesDataManager.VerticesDataManager;

import java.util.ArrayList;
import java.util.List;

public class SceneOutlinerController {
    protected Scene sceneToParse;
    protected VerticesDataManager verticesDataManager;

    public SceneOutlinerController(Scene sceneToParse, VerticesDataManager verticesDataManager) {
        this.sceneToParse = sceneToParse;
        this.verticesDataManager = verticesDataManager;
    }

    protected List<ModelPreview> prepareModelPreviews(){
        List<ModelPreview> result = new ArrayList<>();
        sceneToParse.getModels().forEach(model->{
            ModelPreview preview = new ModelPreview();
            int trianglesCount = 0;
            int texturesCount = 0;
            for (Mesh mesh : model.getMeshes()) {
                trianglesCount+=verticesDataManager.getVertexData(mesh.getVerticesId()).getIndices().length;
                texturesCount+=mesh.getTextures().size();
            }
            trianglesCount/=3;
            preview.setMeshesAmount(model.getMeshes().size());
            preview.setTexturesAmount(texturesCount);
            preview.setPolygonsAmount(trianglesCount);
            preview.setTitle(model.getName());
            preview.update();
            result.add(preview);
        });
        return result;
    }

    public Scene getSceneToParse() {
        return sceneToParse;
    }

    public void setSceneToParse(Scene sceneToParse) {
        this.sceneToParse = sceneToParse;
    }

    public VerticesDataManager getVerticesDataManager() {
        return verticesDataManager;
    }

    public void setVerticesDataManager(VerticesDataManager verticesDataManager) {
        this.verticesDataManager = verticesDataManager;
    }
}
