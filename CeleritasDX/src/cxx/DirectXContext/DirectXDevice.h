//
// Created by Daniil on 30.03.2023.
//
#pragma once

#include <d3d11.h>
#include <dxgi.h>
#include <vector>
#include <iostream>
#include "../Util/StringUtil.h"
#include "../Util/IDestroyableObject.h"
struct TargetDevice{
    std::string name;
    size_t videoMemorySize;
    size_t sharedMemorySize;
    size_t systemMemorySize;
    IDXGIAdapter* adapter;
};

class DirectXDevice : public IDestroyableObject{
private:
    static inline IDXGIFactory* factory = nullptr;
    static inline D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;

private:
    ID3D11Device* device;
    ID3D11DeviceContext* deviceContext;
    IDXGIAdapter* baseAdapter;
public:
    DirectXDevice(ID3D11Device *device, ID3D11DeviceContext *deviceContext, IDXGIAdapter *baseAdapter) : device(device),
                                                                                                         deviceContext(
                                                                                                                 deviceContext),
                                                                                                         baseAdapter(
                                                                                                                 baseAdapter) {}

    void destroy() override {
        device->Release();
        deviceContext->Release();
        destroyed = true;
    }

    ID3D11Device *getDevice()  {
        return device;
    }

    ID3D11DeviceContext *getDeviceContext()  {
        return deviceContext;
    }

    IDXGIAdapter *getBaseAdapter()  {
        return baseAdapter;
    }
    ID3D11RenderTargetView* createRenderTargetView(ID3D11Texture2D* output){
        ID3D11RenderTargetView* result;
        device->CreateRenderTargetView(output, nullptr, &result);
        return result;
    }

    static IDXGIFactory *getFactory() {
        return factory;
    }

public:
    static bool initializeDXContext(){
        HRESULT result = result = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory);
        if(FAILED(result))
        {
            return false;
        }
        return true;
    }
    static void enumerateSupportedDevices(std::vector<TargetDevice>& out){
        IDXGIAdapter* curAdapter;
        ID3D11Device* targetDevice;
        ID3D11DeviceContext* immediateContext;


        std::vector<IDXGIAdapter*> supportedAdapters;
        unsigned int adapterCounter = 0;
        while(!FAILED( factory->EnumAdapters(adapterCounter, &curAdapter))){

                if (!FAILED(D3D11CreateDevice(curAdapter, D3D_DRIVER_TYPE_UNKNOWN, nullptr, 0, &featureLevel, 1, D3D11_SDK_VERSION, &targetDevice,
                                              nullptr, &immediateContext))) {
                    supportedAdapters.push_back(curAdapter);
                    targetDevice->Release();
                    immediateContext->Release();
                    break;
                }

            adapterCounter++;
        }
        for (const auto &item: supportedAdapters){
            DXGI_ADAPTER_DESC adapterDesc;
            item->GetDesc(&adapterDesc);
            TargetDevice curDevice{StringUtil::wcharToString(adapterDesc.Description), adapterDesc.DedicatedVideoMemory, adapterDesc.SharedSystemMemory, adapterDesc.DedicatedSystemMemory, item};
            out.push_back(curDevice);
        }
    }
    static DirectXDevice* createDevice(TargetDevice& device){
        ID3D11Device* targetDevice;
        ID3D11DeviceContext* immediateContext;
        if (SUCCEEDED(D3D11CreateDevice(device.adapter, D3D_DRIVER_TYPE_UNKNOWN, nullptr, 0, &featureLevel, 1, D3D11_SDK_VERSION, &targetDevice,
                                        nullptr, &immediateContext))) {
            return new DirectXDevice(targetDevice, immediateContext, device.adapter);
        }
        return nullptr;
    }
};