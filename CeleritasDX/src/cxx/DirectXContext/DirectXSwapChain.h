//
// Created by Daniil on 30.03.2023.
//
#pragma once
#include "../Util/IDestroyableObject.h"
#include "DirectXDevice.h"
#include "../Window/Window.h"
class DirectXSwapChain : public IDestroyableObject{
private:
    IDXGISwapChain* swapChain;
    DirectXDevice* device;
    Window* window;
public:
    DirectXSwapChain(DirectXDevice *device, Window *window) : device(device), window(window) {
       create();
    }
    void create(){
        DXGI_SWAP_CHAIN_DESC swapChainDesc;
        ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));
        swapChainDesc.BufferCount = 1;
        swapChainDesc.BufferDesc.Width = window->getWidth();
        swapChainDesc.BufferDesc.Height = window->getHeight();
        swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        swapChainDesc.BufferDesc.RefreshRate.Numerator = window->getRefreshRate();
        swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
        swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        swapChainDesc.OutputWindow = window->getWindowWin32Handle();
        swapChainDesc.SampleDesc.Count = 1;
        swapChainDesc.SampleDesc.Quality = 0;
        swapChainDesc.Windowed = !window->isFullScreen();
        if(!SUCCEEDED(DirectXDevice::getFactory()->CreateSwapChain(device->getDevice(), &swapChainDesc, &swapChain))){
            throw std::runtime_error("Failed to create window");
        }
        D3D11_VIEWPORT vp = {};
        vp.Width = window->getWidth();
        vp.Height = window->getHeight();
        vp.MinDepth = 0.0f;
        vp.MaxDepth = 1.0f;
        device->getDeviceContext()->RSSetViewports(1, &vp);
    }

    ID3D11Texture2D* acquireSwapChainTexture(){
        ID3D11Texture2D* result = nullptr;
        if(SUCCEEDED(swapChain->GetBuffer(0, IID_PPV_ARGS(&result)))){
            return result;
        }
        throw std::runtime_error("failed to acquire swap chain image");
    }

    void present(bool vSync){
        swapChain->Present(vSync, NULL);
    }

    void destroy() override {
        if(!destroyed){
            swapChain->Release();
            destroyed = true;
        }
    }
};