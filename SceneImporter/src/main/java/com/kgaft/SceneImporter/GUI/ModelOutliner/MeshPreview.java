package com.kgaft.SceneImporter.GUI.ModelOutliner;

import javax.swing.*;
import java.awt.*;

public class MeshPreview extends JButton {
    private String name;
    private int trianglesAmount;
    private int texturesAmount;
    private JPanel root;
    private JLabel titleLabel = new JLabel();
    private JLabel trianglesAmountLabel = new JLabel();
    private JLabel texturesAmountLabel = new JLabel();

    public MeshPreview(String name, int trianglesAmount, int texturesAmount) {
        this.name = name;
        this.trianglesAmount = trianglesAmount;
        this.texturesAmount = texturesAmount;
        titleLabel.setFont(new Font("Cantarell", Font.BOLD, 20));
        setMaximumSize(new Dimension(-1, 100));
        setPreferredSize(new Dimension(-1, 100));
        setMinimumSize(new Dimension(-1, 100));
        root = new JPanel();
        root.setMaximumSize(new Dimension(-1, 100));
        setLayout(new GridLayout(1,1));
        add(root);
        root.setLayout(new GridLayout(-1, 1));
        root.add(titleLabel);
        root.add(trianglesAmountLabel);
        root.add(texturesAmountLabel);

    }
    public void update(){
        titleLabel.setText(name);
        trianglesAmountLabel.setText("Triangles: "+trianglesAmount);
        texturesAmountLabel.setText("Textures: "+ texturesAmount);
    }
}
