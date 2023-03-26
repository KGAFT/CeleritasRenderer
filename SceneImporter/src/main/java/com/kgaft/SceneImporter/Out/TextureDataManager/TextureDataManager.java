package com.kgaft.SceneImporter.Out.TextureDataManager;

import com.kgaft.SceneImporter.Data.Model.Model;
import com.kgaft.SceneImporter.Data.Texture.Texture;

import java.io.File;
import java.lang.module.FindException;
import java.nio.ByteBuffer;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;

import static org.lwjgl.stb.STBImage.STBI_rgb_alpha;
import static org.lwjgl.stb.STBImage.stbi_load;

public class TextureDataManager {
    private HashMap<String, TextureData> loadedTextures = new HashMap<>();
    private int idCounter = 0;
    public void parseTexturesFromModel(Model model){
        model.getMeshes().forEach(mesh -> {
            List<Integer> textureIds = new ArrayList<>();
            mesh.getTextures().forEach(element->{
                TextureData data = loadedTextures.get(element.getPath());
                if(data==null){
                    data = loadTexture(element);
                }
                if(data!=null){
                    textureIds.add(data.getId());
                }
            });
            mesh.setLoadedTextures(textureIds);
        });
    }
    private TextureData loadTexture(Texture texture){
        int[] width = new int[1];
        int[] height = new int[1];
        int[] imageChannels = new int[1];
        if(new File(texture.getPath()).isFile()){
            ByteBuffer textureData = stbi_load(texture.getPath(), width, height, imageChannels, STBI_rgb_alpha);
            TextureData data = new TextureData(textureData, texture.getTextureType(), idCounter);
            data.setWidth(width[0]);
            data.setHeight(height[0]);
            data.setChannelsAmount(imageChannels[0]);
            idCounter++;
            loadedTextures.put(texture.getPath(), data);
        }
        return null;
    }

    public HashMap<String, TextureData> getLoadedTextures() {
        return loadedTextures;
    }
}
