#pragma once
#include <d3d11.h>

class Renderer
{
public:
    bool InitD3D(HWND hWnd);
    void Clear();
    void Present();
    void Cleanup();

    ID3D11Device* GetDevice() { return dev; }
    ID3D11DeviceContext* GetDeviceContext() { return devcon; }

private:
    IDXGISwapChain* swapchain = nullptr;
    ID3D11Device* dev = nullptr;
    ID3D11DeviceContext* devcon = nullptr;
    ID3D11RenderTargetView* backbuffer = nullptr;
};
