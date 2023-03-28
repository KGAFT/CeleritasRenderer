package com.kgaft.SceneImporter.GUI;

import com.formdev.flatlaf.FlatDarkLaf;
import com.kgaft.SceneImporter.Data.Scene;
import com.kgaft.SceneImporter.GUI.SceneOutliner.SceneOutliner;
import com.kgaft.SceneImporter.ModelLoader.ModelLoader;
import com.kgaft.SceneImporter.Out.VerticesDataManager.VerticesDataManager;

import javax.swing.*;

public class WindowManager {
    public WindowManager() throws UnsupportedLookAndFeelException {
        FlatDarkLaf.setup();
        UIManager.setLookAndFeel(new FlatDarkLaf());
        Scene scene = new Scene();
        VerticesDataManager verticesDataManager = new VerticesDataManager();
        ModelLoader loader = new ModelLoader(verticesDataManager);
        scene.getModels().add(loader.loadModel("D:/documents/projects/GitHub/C++/CeleritasEngine/UnderWater/scene.gltf"));
        new SceneOutliner(verticesDataManager, scene).init();
    }
}
