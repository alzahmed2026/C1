#include "Renderer.h"

bool Renderer::InitD3D(HWND hWnd)
{
    DXGI_SWAP_CHAIN_DESC scd = {};
    scd.BufferCount = 1;
    scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    scd.OutputWindow = hWnd;
    scd.SampleDesc.Count = 1;
    scd.Windowed = TRUE;

    D3D11CreateDeviceAndSwapChain(
        NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0, NULL, 0,
        D3D11_SDK_VERSION, &scd, &swapchain, &dev, NULL, &devcon);

    ID3D11Texture2D* pBackBuffer;
    swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
    dev->CreateRenderTargetView(pBackBuffer, NULL, &backbuffer);
    pBackBuffer->Release();

    devcon->OMSetRenderTargets(1, &backbuffer, NULL);
    return true;
}

void Renderer::Clear()
{
    float color[4] = { 0.2f, 0.2f, 0.2f, 1.0f };
    devcon->ClearRenderTargetView(backbuffer, color);
}

void Renderer::Present()
{
    swapchain->Present(1, 0);
}

void Renderer::Cleanup()
{
    swapchain->Release();
    backbuffer->Release();
    dev->Release();
    devcon->Release();
}
