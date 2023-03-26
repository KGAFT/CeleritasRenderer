package com.kgaft.SceneImporter.GUI.SceneOutliner;

import javax.swing.*;
import java.awt.*;

public class SceneOutliner extends JFrame{
    private JPanel root;
    private JButton addModelButton;
    private JScrollPane modelsScroller;
    private JPanel modelsOutliner;

    public SceneOutliner() {
        setSize(400, 800);
        setTitle("Scene outliner");
        setDefaultCloseOperation(DISPOSE_ON_CLOSE);
        modelsOutliner.setLayout(new GridLayout(-1, 1));
        add(root);
        setVisible(true);

    }
    public void init(){
        modelsOutliner.add(new ModelPreview("Hello model", 12, 2, 3));
    }
}
