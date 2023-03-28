package com.kgaft.SceneImporter;


import com.kgaft.SceneImporter.Data.Model.Model;
import com.kgaft.SceneImporter.Data.Scene;
import com.kgaft.SceneImporter.GUI.WindowManager;
import com.kgaft.SceneImporter.ModelLoader.ModelLoader;
import com.kgaft.SceneImporter.Out.SceneWriter;

import javax.swing.*;
import java.io.File;
import java.io.FileWriter;
import java.io.IOException;

public class Main {
    public static void main(String[] args) throws IOException {
        SceneWriter writer = new SceneWriter();
        ModelLoader loader = new ModelLoader(writer.getVerticesDataManager());
        Model model = loader.loadModel("D:/documents/projects/GitHub/C++/KGAFTEngine/models/UnderWater/scene.gltf");
        Scene scene =  new Scene();
        scene.getModels().add(model);
        writer.loadTextures(scene);
        FileWriter writer1 = new FileWriter("C:/Users/Daniil/Desktop/model.sc");
        writer.writeSceneObjectsInfo(scene, writer1);
        writer.writeVerticesDataToFile(writer1);
        writer.writeTextures(writer1);
        writer1.flush();
    }
}