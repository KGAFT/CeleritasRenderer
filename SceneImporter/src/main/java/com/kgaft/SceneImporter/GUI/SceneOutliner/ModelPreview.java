package com.kgaft.SceneImporter.GUI.SceneOutliner;

import javax.swing.*;
import java.awt.*;

public class ModelPreview extends JButton {
    private String title;
    private int polygonsAmount;
    private int texturesAmount;
    private int meshesAmount;
    private JPanel root;
    private JLabel titleLabel;
    private JLabel polygonsLabel;
    private JLabel texturesLabel;
    private JLabel meshesAmountLabel;
    public ModelPreview(String title, int meshesAmount, int polygonsAmount, int texturesAmount) {
        this.title = title;
        this.polygonsAmount = polygonsAmount;
        this.texturesAmount = texturesAmount;
        this.meshesAmount = meshesAmount;
        setMaximumSize(new Dimension(-1, 100));
        setPreferredSize(new Dimension(-1, 100));
        setMinimumSize(new Dimension(-1, 100));
        root = new JPanel();
        root.setMaximumSize(new Dimension(-1, 100));
        setLayout(new GridLayout(1,1));
        add(root);
        root.setLayout(new GridLayout(-1, 1));
        initLabels();
    }
    public void update(){
        titleLabel.setText(title);

        polygonsLabel.setText("Triangles: "+polygonsAmount);
        texturesLabel.setText("Textures amount: "+texturesAmount);
        meshesAmountLabel.setText("Meshes: "+meshesAmount);
        root.repaint();
    }
    private void initLabels(){
        titleLabel = new JLabel(title);
        titleLabel.setFont(new Font("Cantarell", Font.BOLD, 20));
        polygonsLabel = new JLabel("Triangles: "+polygonsAmount);
        texturesLabel = new JLabel("Textures amount: "+texturesAmount);
        meshesAmountLabel = new JLabel("Meshes: "+meshesAmount);
        root.add(titleLabel);
        root.add(meshesAmountLabel);
        root.add(polygonsLabel);
        root.add(texturesLabel);
    }

    public String getTitle() {
        return title;
    }

    public void setTitle(String title) {
        this.title = title;
    }

    public int getPolygonsAmount() {
        return polygonsAmount;
    }

    public void setPolygonsAmount(int polygonsAmount) {
        this.polygonsAmount = polygonsAmount;
    }

    public int getTexturesAmount() {
        return texturesAmount;
    }

    public void setTexturesAmount(int texturesAmount) {
        this.texturesAmount = texturesAmount;
    }

    public int getMeshesAmount() {
        return meshesAmount;
    }

    public void setMeshesAmount(int meshesAmount) {
        this.meshesAmount = meshesAmount;
    }
}
