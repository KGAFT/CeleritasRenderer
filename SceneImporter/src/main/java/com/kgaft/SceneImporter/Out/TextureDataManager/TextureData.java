package com.kgaft.SceneImporter.Out.TextureDataManager;

import com.kgaft.SceneImporter.Data.Texture.TextureType;

import java.nio.ByteBuffer;

public class TextureData {
    private ByteBuffer data;
    private TextureType type;
    private int id;
    private int width;
    private int height;
    private int channelsAmount;
    public TextureData(ByteBuffer data, TextureType type, int id) {
        this.data = data;
        this.type = type;
        this.id = id;
    }

    public ByteBuffer getData() {
        return data;
    }

    public void setData(ByteBuffer data) {
        this.data = data;
    }

    public TextureType getType() {
        return type;
    }

    public void setType(TextureType type) {
        this.type = type;
    }

    public int getId() {
        return id;
    }

    public void setId(int id) {
        this.id = id;
    }

    public int getWidth() {
        return width;
    }

    public void setWidth(int width) {
        this.width = width;
    }

    public int getHeight() {
        return height;
    }

    public void setHeight(int height) {
        this.height = height;
    }

    public int getChannelsAmount() {
        return channelsAmount;
    }

    public void setChannelsAmount(int channelsAmount) {
        this.channelsAmount = channelsAmount;
    }
}
