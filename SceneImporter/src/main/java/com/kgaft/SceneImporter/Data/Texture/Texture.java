package com.kgaft.SceneImporter.Data.Texture;

public class Texture {
    private String path;
    private TextureType textureType;

    public Texture(String path, TextureType textureType) {
        this.path = path;
        this.textureType = textureType;
    }

    public String getPath() {
        return path;
    }

    public void setPath(String path) {
        this.path = path;
    }

    public TextureType getTextureType() {
        return textureType;
    }

    public void setTextureType(TextureType textureType) {
        this.textureType = textureType;
    }
}
