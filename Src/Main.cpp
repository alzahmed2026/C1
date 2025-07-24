#include <windows.h>
#include "Renderer.h"
#include "EditorUI.h"

// المتغيرات
HWND hWnd;
Renderer* renderer;
EditorUI* editorUI;

// نافذة Windows
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hwnd, uMsg, wParam, lParam))
        return true;

    switch (uMsg)
    {
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow)
{
    // تسجيل النافذة
    WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, WindowProc, 0, 0,
        GetModuleHandle(NULL), NULL, NULL, NULL, NULL, L"GameEngineWindow", NULL };
    RegisterClassEx(&wc);
    hWnd = CreateWindow(wc.lpszClassName, L"My Game Engine", WS_OVERLAPPEDWINDOW,
        100, 100, 1280, 720, NULL, NULL, wc.hInstance, NULL);

    renderer = new Renderer();
    renderer->InitD3D(hWnd);

    editorUI = new EditorUI();
    editorUI->Init(renderer->GetDevice(), renderer->GetDeviceContext(), hWnd);

    ShowWindow(hWnd, nCmdShow);

    // حلقة الرسائل
    MSG msg = {};
    while (msg.message != WM_QUIT)
    {
        if (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else
        {
            renderer->Clear();

            editorUI->Render();

            renderer->Present();
        }
    }

    editorUI->Cleanup();
    renderer->Cleanup();

    UnregisterClass(wc.lpszClassName, wc.hInstance);
    return 0;
}
