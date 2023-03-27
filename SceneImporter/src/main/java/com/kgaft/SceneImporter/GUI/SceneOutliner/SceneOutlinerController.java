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
    protected List<ModelPreview> prepareModelPreviews(){
        List<ModelPreview> result = new ArrayList<>();
        sceneToParse.getModels().forEach(model->{
            ModelPreview preview = new ModelPreview();
            int trianglesCount = 0;
            for (Mesh mesh : model.getMeshes()) {
                verticesDataManager.
            }
        });
    }
}
