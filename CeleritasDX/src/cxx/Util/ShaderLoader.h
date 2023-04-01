//
// Created by Daniil on 01.04.2023.
//
#pragma once

#include <d3dcompiler.h>
#include <d3d11.h>

#include "../DirectXContext/DirectXShader.h"
#include "../DirectXContext/DirectXDevice.h"
#include "ShaderConfParser.h"

class ShaderLoader{
public:
    static DirectXShader* loadShader(const char* pathToDir, DirectXDevice* device){
       std::vector<ShaderInfo> shadersInfo = ShaderConfParser::parseShader(pathToDir);
        ID3D11VertexShader* vertexShader = nullptr;
        ID3D11PixelShader* pixelShader = nullptr;
        ID3D11GeometryShader* geometryShader = nullptr;
        ID3DBlob * vertexShaderBlob = nullptr;
        ID3DBlob * pixelShaderBlob = nullptr;
        ID3DBlob * geometryShaderBlob = nullptr;
        ID3DBlob * errorBlob;
        for (auto &item: shadersInfo){
            switch (item.type) {
                case VERTEX_SHADER:
                    vertexShader = loadVertexShader(item, &vertexShaderBlob, device);
                    break;
                case FRAGMENT_SHADER:
                    pixelShader = loadPixelShader(item, &pixelShaderBlob, device);
                    break;
                case GEOMETRY_SHADER:
                    geometryShader = loadGeometryShader(item, &geometryShaderBlob, device);
                    break;
                default:
                    break;
            }
        }
        DirectXShader* result = new DirectXShader()
    }
    static ID3D11VertexShader* loadVertexShader(ShaderInfo& info, ID3DBlob** outputData, DirectXDevice* device){
        ID3DBlob* errblob = nullptr;
        ID3D11VertexShader* vertexShader = nullptr;
        D3DCompileFromFile(reinterpret_cast<const WCHAR *>(info.path.c_str()), nullptr, nullptr, "vsmain", "vs_5_0", NULL, NULL, outputData,
                           &errblob);
        if(errblob->GetBufferSize() > 0){
            for (int i = 0; i < errblob->GetBufferSize(); i += sizeof(char)) {
                std::cerr << *(((char *) errblob->GetBufferPointer()) + i);
            }
            std::cerr << std::endl;
            throw std::runtime_error("");
        }
        device->getDevice()->CreateVertexShader(outputData[0]->GetBufferPointer(), outputData[0]->GetBufferSize(), nullptr,
                                   &vertexShader);
        return vertexShader;
    }
    static ID3D11PixelShader* loadPixelShader(ShaderInfo& info, ID3DBlob** outputData,  DirectXDevice* device){
        ID3DBlob* errblob = nullptr;
        ID3D11PixelShader* pixelShader = nullptr;
        D3DCompileFromFile(reinterpret_cast<const WCHAR *>(info.path.c_str()), nullptr, nullptr, "vsmain", "vs_5_0", NULL, NULL, outputData,
                           &errblob);
        if(errblob->GetBufferSize() > 0){
            for (int i = 0; i < errblob->GetBufferSize(); i += sizeof(char)) {
                std::cerr << *(((char *) errblob->GetBufferPointer()) + i);
            }
            std::cerr << std::endl;
            throw std::runtime_error("");
        }
        device->getDevice()->CreatePixelShader(outputData[0]->GetBufferPointer(), outputData[0]->GetBufferSize(), nullptr,
                                                &pixelShader);
        return pixelShader;
    }
    static ID3D11GeometryShader* loadGeometryShader(ShaderInfo& info, ID3DBlob** outputData,  DirectXDevice* device){
        ID3DBlob * errblob = nullptr;
        ID3D11GeometryShader* geometryShader = nullptr;
        D3DCompileFromFile(reinterpret_cast<const WCHAR *>(info.path.c_str()), nullptr, nullptr, "vsmain", "vs_5_0", NULL, NULL, outputData,
                           &errblob);
        if(errblob->GetBufferSize() > 0){
            for (int i = 0; i < errblob->GetBufferSize(); i += sizeof(char)) {
                std::cerr << *(((char *) errblob->GetBufferPointer()) + i);
            }
            std::cerr << std::endl;
            throw std::runtime_error("");
        }
        device->getDevice()->CreateGeometryShader(outputData[0]->GetBufferPointer(), outputData[0]->GetBufferSize(), nullptr,
                                               &geometryShader);
        return geometryShader;
    }
};