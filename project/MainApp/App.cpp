#include "general.h"
#include "basewin.h"
#include "App.h"

HRESULT MainWindow::CreateGraphicsResources()
{
    HRESULT hResult = S_OK;

    if (pRenderTarget == NULL)
    {
        /* window */
        RECT clientRect;
        GetClientRect(m_hwnd, &clientRect);

        D2D1_SIZE_U size = D2D1::SizeU(
            clientRect.right - clientRect.left,
            clientRect.bottom - clientRect.top
        );
        
        /* render */
        hResult = pFactory->CreateHwndRenderTarget(
            D2D1::RenderTargetProperties(
                D2D1_RENDER_TARGET_TYPE_DEFAULT, 
                D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED)),
            D2D1::HwndRenderTargetProperties(m_hwnd, size),
            &pRenderTarget
        );

        if (SUCCEEDED(hResult))
        {
            /* background  brush */
            ID2D1GradientStopCollection* pGradientStops = NULL;
            D2D1_GRADIENT_STOP gradientStops[2];
            gradientStops[0].color = D2D1::ColorF(D2D1::ColorF::Blue, 1);
            gradientStops[0].position = 0.0f;
            gradientStops[1].color = D2D1::ColorF(D2D1::ColorF::Violet, 1);
            gradientStops[1].position = 1.0f;

            hResult = pRenderTarget->CreateGradientStopCollection(
                gradientStops,
                2,
                D2D1_GAMMA_2_2,
                D2D1_EXTEND_MODE_CLAMP,
                &pGradientStops
            );

            if (SUCCEEDED(hResult))
            {
                hResult = pRenderTarget->CreateLinearGradientBrush(
                    D2D1::LinearGradientBrushProperties(
                        D2D1::Point2F(clientRect.left, clientRect.top),
                        D2D1::Point2F(clientRect.right, clientRect.bottom)),
                    pGradientStops,
                    &pBackgroudnGradientBrush
                );
            }

            /* border brush */
            pGradientStops = NULL;
            gradientStops[0].color = D2D1::ColorF(D2D1::ColorF::Violet, 1);
            gradientStops[0].position = 0.0f;
            gradientStops[1].color = D2D1::ColorF(D2D1::ColorF::Blue, 1);
            gradientStops[1].position = 1.0f;

            hResult = pRenderTarget->CreateGradientStopCollection(
                gradientStops,
                2,
                D2D1_GAMMA_2_2,
                D2D1_EXTEND_MODE_CLAMP,
                &pGradientStops
            );

            if (SUCCEEDED(hResult))
            {
                hResult = pRenderTarget->CreateLinearGradientBrush(
                    D2D1::LinearGradientBrushProperties(
                        D2D1::Point2F(clientRect.left, clientRect.top),
                        D2D1::Point2F(clientRect.right, clientRect.bottom)),
                    pGradientStops,
                    &pWindowBorderBrush
                );
            }
        }
    }

    return hResult;
}

void MainWindow::DiscardGraphicsResources()
{
    SafeRelease(&pBackgroudnGradientBrush);
    SafeRelease(&pWindowBorderBrush);
    SafeRelease(&pRenderTarget);
}

void MainWindow::OnPaint()
{
    HRESULT hResult = CreateGraphicsResources();

    if (SUCCEEDED(hResult))
    {
        /* prepare for drawing */
        //PAINTSTRUCT paintStruct;
        //BeginPaint(m_hwnd, &paintStruct);
        pRenderTarget->BeginDraw();
        pRenderTarget->Clear();

        /* get sizes */
        D2D1_SIZE_F clientSizes = pRenderTarget->GetSize();

        /* draw window backgorund and border */
        D2D1_ROUNDED_RECT roundedWindowRect = D2D1::RoundedRect(
            D2D1::RectF(0, 0, clientSizes.width, clientSizes.height),
            30.f,
            30.f
        );

        pRenderTarget->FillRoundedRectangle(&roundedWindowRect, pBackgroudnGradientBrush);
        pRenderTarget->DrawRoundedRectangle(&roundedWindowRect, pWindowBorderBrush, 2.0f, NULL);

        hResult = pRenderTarget->EndDraw();
        if (FAILED(hResult) || hResult == D2DERR_RECREATE_TARGET)
        {
            DiscardGraphicsResources();
        }
        //EndPaint(m_hwnd, &paintStruct);
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

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR, int nCmdShow)
{
    MainWindow win;

    BOOL creationResult = win.Create(
        L"USB Monitor",
        WS_POPUP | WS_EX_TOPMOST,
        WS_EX_LAYERED, //for transparent d2d1 clear
        (SCREEN_WIDTH - MainWindow::startWndWidth) / 2,
        (SCREEN_HEIGHT - MainWindow::startWndHeight) / 2,
        MainWindow::startWndWidth,
        MainWindow::startWndHeight
    );

    if (!creationResult)
        return EXIT_FAILURE;

    ShowWindow(win.Window(), nCmdShow);

    /* message loop */
    MSG msg = { };
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return static_cast<int>(msg.message);
}

MainWindow::MainWindow()
{
    pFactory = nullptr;
    pRenderTarget = nullptr;
    pBackgroudnGradientBrush = nullptr;
    pWindowBorderBrush = nullptr;
}
