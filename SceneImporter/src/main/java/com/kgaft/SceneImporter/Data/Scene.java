package com.kgaft.SceneImporter.Data;

import com.kgaft.SceneImporter.Data.Model.Model;

import java.util.ArrayList;
import java.util.List;
public class Scene {
    private List<Model> models = new ArrayList<>();

    public List<Model> getModels() {
        return models;
    }

    public void setModels(List<Model> models) {
        this.models = models;
    }

}
