//
// Created by Daniil on 31.03.2023.
//
#pragma once

#include <d3d11.h>
#include "DirectXDevice.h"

class DirectXShader{
    friend class ShaderLoader;
private:
    DirectXDevice* device = nullptr;
    ID3D11VertexShader *vertexShader = nullptr;
    ID3D11PixelShader *pixelShader = nullptr;
    ID3D11GeometryShader* geometryShader = nullptr;

    ID3DBlob* vertexShaderData = nullptr;
    ID3DBlob* pixelShaderData = nullptr;
    ID3DBlob* geometryShaderData = nullptr;
private:
    DirectXShader(DirectXDevice *device, ID3D11VertexShader *vertexShader, ID3D11PixelShader *pixelShader,
                  ID3D11GeometryShader *geometryShader, ID3DBlob *vertexShaderData, ID3DBlob *pixelShaderData,
                  ID3DBlob *geometryShaderData) : device(device), vertexShader(vertexShader), pixelShader(pixelShader),
                                                  geometryShader(geometryShader), vertexShaderData(vertexShaderData),
                                                  pixelShaderData(pixelShaderData),
                                                  geometryShaderData(geometryShaderData) {}

public:
    void attach(){
        if(pixelShader!=nullptr){
            device->getDeviceContext()->PSSetShader(pixelShader, nullptr, 0);
        }
        if(vertexShader!=nullptr){
            device->getDeviceContext()->VSSetShader(vertexShader, nullptr, 0);
        }
        if(geometryShader!=nullptr){
            device->getDeviceContext()->GSSetShader(geometryShader, nullptr, 0);
        }
    }
};
