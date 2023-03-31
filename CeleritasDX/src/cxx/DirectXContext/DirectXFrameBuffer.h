#pragma once

#include <d3d11.h>
#include "DirectXDevice.h"

class DirectXFrameBuffer : public IDestroyableObject {
private:
    std::vector<ID3D11RenderTargetView *> renderTargetViews;
    DirectXDevice *device;
    ID3D11Texture2D *depthTexture = nullptr;
    ID3D11DepthStencilState *depthStencilState = nullptr;
    ID3D11DepthStencilView *depthStencilView = nullptr;
public:
    DirectXFrameBuffer(ID3D11Texture2D **outputTargets, DirectXDevice *device, unsigned int targetCount, bool depthTest)
            : device(
            device) {
        create(outputTargets, device, targetCount);
        if (depthTest) {

            D3D11_TEXTURE2D_DESC desc;
            outputTargets[0]->GetDesc(&desc);
            createDepthResources(desc.Width, desc.Height);
        }
    }

    void recreate(ID3D11Texture2D **outputTargets, unsigned int targetCount) {
        bool depth = depthTexture != nullptr;
        destroy();
        destroyed = false;
        create(outputTargets, device, targetCount);
        if (depth) {
            D3D11_TEXTURE2D_DESC desc;
            outputTargets[0]->GetDesc(&desc);
            createDepthResources(desc.Width, desc.Height);
        }
    }
    void clear(float r, float g, float b, float a){
        float color[]{r,g,b,a};
        for (const auto &item: renderTargetViews){
            device->getDeviceContext()->ClearRenderTargetView(item, color);
        }
        device->getDeviceContext()->ClearDepthStencilView(depthStencilView,  D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1, 0);

    }
    void bind() {
        if (depthTexture != nullptr) {
            device->getDeviceContext()->OMSetDepthStencilState(depthStencilState, 1);
            device->getDeviceContext()->OMSetRenderTargets(renderTargetViews.size(), renderTargetViews.data(),
                                                           depthStencilView);
        } else {
            device->getDeviceContext()->OMSetDepthStencilState(nullptr, 0);
            device->getDeviceContext()->OMSetRenderTargets(renderTargetViews.size(), renderTargetViews.data(), nullptr);
        }
    }

    ID3D11Texture2D *getDepthTexture() {
        return depthTexture;
    }

    void destroy() override {
        IDestroyableObject::destroy();
        for (const auto &item: renderTargetViews) {
            item->Release();
        }
        renderTargetViews.clear();
        depthStencilView->Release();
        depthStencilView->Release();
        depthTexture->Release();
        depthStencilView = nullptr;
        depthStencilState = nullptr;
        depthTexture = nullptr;
    }

private:
    void create(ID3D11Texture2D **outputTargets, DirectXDevice *device, unsigned int targetCount) {
        ID3D11RenderTargetView *rtv;
        for (int i = 0; i < targetCount; ++i) {
            ID3D11RenderTargetView *rtv = device->createRenderTargetView(outputTargets[i]);

            renderTargetViews.push_back(rtv);
        }

    }

    void createDepthResources(int width, int height) {

        D3D11_TEXTURE2D_DESC depthTexDesc = {};
        depthTexDesc.Width = width;
        depthTexDesc.Height = height;
        depthTexDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
        depthTexDesc.Usage = D3D11_USAGE_DEFAULT;
        depthTexDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
        depthTexDesc.MipLevels = 1;
        depthTexDesc.SampleDesc.Count = 1;
        depthTexDesc.SampleDesc.Quality = 0;
        depthTexDesc.MiscFlags = 0;
        depthTexDesc.ArraySize = 1;
        depthTexDesc.CPUAccessFlags = 0;
        device->getDevice()->CreateTexture2D(&depthTexDesc, nullptr, &depthTexture);

        D3D11_DEPTH_STENCIL_DESC dsDesc;

        dsDesc.DepthEnable = true;
        dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
        dsDesc.DepthFunc = D3D11_COMPARISON_LESS;

        D3D11_DEPTH_STENCILOP_DESC defaultStencilOp = // a stencil operation structure, again does not really matter since stencil testing is turned off
                {D3D11_STENCIL_OP_KEEP, D3D11_STENCIL_OP_KEEP, D3D11_STENCIL_OP_KEEP, D3D11_COMPARISON_ALWAYS};

        dsDesc.StencilEnable = false;
        dsDesc.FrontFace = defaultStencilOp;
        dsDesc.BackFace = defaultStencilOp;
        device->getDevice()->CreateDepthStencilState(&dsDesc, &depthStencilState);
        D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
        descDSV.Format = DXGI_FORMAT_D32_FLOAT_S8X24_UINT;
        descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
        descDSV.Texture2D.MipSlice = 0;
        device->getDevice()->CreateDepthStencilView(depthTexture, &descDSV, &depthStencilView);

    }
};