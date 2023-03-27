package com.kgaft.SceneImporter.GUI.SceneOutliner;

import com.kgaft.SceneImporter.Data.Scene;
import com.kgaft.SceneImporter.Out.VerticesDataManager.VerticesDataManager;

import javax.swing.*;
import java.awt.*;

public class SceneOutliner extends JFrame{
    private JPanel root;
    private JButton addModelButton;
    private JScrollPane modelsScroller;
    private JPanel modelsOutliner;

    private SceneOutlinerController controller;

    public SceneOutliner(VerticesDataManager dataManager, Scene scene) {
        controller = new SceneOutlinerController(scene, dataManager);
        setSize(400, 800);
        setTitle("Scene outliner");
        setDefaultCloseOperation(DISPOSE_ON_CLOSE);
        modelsOutliner.setLayout(new GridLayout(-1, 1));
        add(root);
        setVisible(true);

    }
    public void init(){
        refresh();
    }
    public void refresh(){
        modelsOutliner.removeAll();
        controller.prepareModelPreviews().forEach(preview->{
            modelsOutliner.add(preview);
        });
    }
}
