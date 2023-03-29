package com.kgaft.SceneImporter;


import com.kgaft.SceneImporter.Data.Model.Model;
import com.kgaft.SceneImporter.Data.Scene;
import com.kgaft.SceneImporter.Data.Texture.Texture;
import com.kgaft.SceneImporter.Data.Texture.TextureType;
import com.kgaft.SceneImporter.GUI.WindowManager;
import com.kgaft.SceneImporter.ModelLoader.ModelLoader;
import com.kgaft.SceneImporter.Out.SceneWriter;

import javax.swing.*;
import java.io.File;
import java.io.FileWriter;
import java.io.IOException;
import java.util.ArrayList;
import java.util.List;

public class Main {
    public static void main(String[] args) throws IOException {
        SceneWriter writer = new SceneWriter();
        ModelLoader loader = new ModelLoader(writer.getVerticesDataManager());
        Model model = loader.loadModel("D:/documents/projects/GitHub/C++/KGAFTEngine/models/UnderWater/scene.gltf");

        List<Texture> mainMeshTextures = new ArrayList<>();
        mainMeshTextures.add(new Texture("D:/documents/projects/GitHub/C++/CeleritasEngine/UnderWater/textures/Body_baseColor.png", TextureType.ALBEDO_TEXTURE));
        mainMeshTextures.add(new Texture("D:/documents/projects/GitHub/C++/CeleritasEngine/UnderWater/textures/Body_metallicRoughness.png", TextureType.METALLIC_ROUGHNESS_TEXTURE));
        mainMeshTextures.add(new Texture("D:/documents/projects/GitHub/C++/CeleritasEngine/UnderWater/textures/Body_normal.png", TextureType.NORMAL_MAP_TEXTURE));
        List<Texture> secondMeshTextures = new ArrayList<>();
        secondMeshTextures.add(new Texture("D:/documents/projects/GitHub/C++/CeleritasEngine/UnderWater/textures/LightBulb_a_baseColor.png", TextureType.ALBEDO_TEXTURE));
        secondMeshTextures.add(new Texture("D:/documents/projects/GitHub/C++/CeleritasEngine/UnderWater/textures/LightBulb_a_metallicRoughness.png", TextureType.METALLIC_ROUGHNESS_TEXTURE));
        secondMeshTextures.add(new Texture("D:/documents/projects/GitHub/C++/CeleritasEngine/UnderWater/textures/LightBulb_a_normal.png", TextureType.NORMAL_MAP_TEXTURE));
        model.getMeshes().get(0).setTextures(mainMeshTextures);
        model.getMeshes().get(1).setTextures(mainMeshTextures);
        model.getMeshes().get(2).setTextures(mainMeshTextures);
        model.getMeshes().get(3).setTextures(mainMeshTextures);
        model.getMeshes().get(4).setTextures(secondMeshTextures);
        model.getMeshes().get(5).setTextures(secondMeshTextures);
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