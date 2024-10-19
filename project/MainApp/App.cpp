#include "general.h"
#include "App.h"

HRESULT MainWindow::CreateGraphicsResources()
{
    HRESULT hResult = S_OK;

    if (pRenderTarget == NULL)
    {
        RECT clientRect;
        GetClientRect(m_hwnd, &clientRect);

        D2D1_SIZE_U size = D2D1::SizeU(clientRect.right, clientRect.bottom);

        hResult = pFactory->CreateHwndRenderTarget(
            D2D1::RenderTargetProperties(),
            D2D1::HwndRenderTargetProperties(m_hwnd, size),
            &pRenderTarget
        );

        if (SUCCEEDED(hResult))
        {
            //creation

            if (SUCCEEDED(hResult))
            {
                //first usage
            }
        }
    }

    return hResult;
}

void MainWindow::DiscardGraphicsResources()
{
    SafeRelease(&pRenderTarget);
}

void MainWindow::OnPaint()
{
    HRESULT hResult = CreateGraphicsResources();

    if (SUCCEEDED(hResult))
    {
        PAINTSTRUCT paintStruct;
        BeginPaint(m_hwnd, &paintStruct);
        pRenderTarget->BeginDraw();

        pRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::SkyBlue));

        hResult = pRenderTarget->EndDraw();
        if (FAILED(hResult) || hResult == D2DERR_RECREATE_TARGET)
        {
            DiscardGraphicsResources();
        }
        EndPaint(m_hwnd, &paintStruct);
    }
}

void MainWindow::Resize()
{
    if (pRenderTarget != NULL)
    {
        RECT clientRect;
        GetClientRect(m_hwnd, &clientRect);

        D2D1_SIZE_U size = D2D1::SizeU(clientRect.right, clientRect.bottom);

        pRenderTarget->Resize(size);
        InvalidateRect(m_hwnd, NULL, FALSE);
    }
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR, int nCmdShow)
{
    MainWindow win;

    if (!win.Create(L"Circle", WS_OVERLAPPEDWINDOW))
    {
        return 0;
    }

    ShowWindow(win.Window(), nCmdShow);

    MSG msg = { };
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}

LRESULT MainWindow::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
        case WM_CREATE:
            if (FAILED(D2D1CreateFactory(
                D2D1_FACTORY_TYPE_SINGLE_THREADED, &pFactory)))
            {
                return -1;  // Fail CreateWindowEx.
            }
            return 0;

        case WM_DESTROY:
            DiscardGraphicsResources();
            SafeRelease(&pFactory);
            PostQuitMessage(0);
            return 0;

        case WM_PAINT:
            OnPaint();
            return 0;

        case WM_SIZE:
            Resize();
            return 0;
    }

    return DefWindowProc(m_hwnd, uMsg, wParam, lParam);
}