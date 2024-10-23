#define _CRT_SECURE_NO_WARNINGS

#include "general.h"
#include "myCircleButton.h"

/* Startup code */

bool MyCircleButton::RegisterCustomButtonClass(HINSTANCE hInstance)
{
    WNDCLASS windowClass = {};
    windowClass.lpfnWndProc = StaticCircleButtonProc;
    windowClass.hInstance = hInstance;
    windowClass.lpszClassName = MyCircleButton::className;

    return RegisterClass(&windowClass);
}

LRESULT CALLBACK MyCircleButton::StaticCircleButtonProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    MyCircleButton* pThis = NULL;

    if (uMsg == WM_CREATE)
    {
        CREATESTRUCT* pCreate = (CREATESTRUCT*)lParam;
        pThis = (MyCircleButton*)pCreate->lpCreateParams;
        SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)pThis);

        pThis->handler = hwnd;
    }
    else
    {
        pThis = (MyCircleButton*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
    }

    if (pThis)
    {
        return pThis->CircleButtonProc(uMsg, wParam, lParam);
    }
    else
    {
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
}

/* Initialization */

MyCircleButton::MyCircleButton(HWND parent, ID2D1Factory* AppFactory, int ID, int posX, int posY, int radius, D2D1_COLOR_F fillColor, D2D1_COLOR_F borderColor)
{
    this->hParent = parent;
    this->menuID = ID;
    this->pAppFactory = AppFactory;
    this->fillColor = fillColor;
    this->borderColor = borderColor;
    this->radius = radius;

    this->handler = CreateWindow(
        MyCircleButton::className, L"",
        WS_CHILD | WS_VISIBLE,
        posX, posY, radius, radius,
        parent, NULL, NULL, this
    );

    if (!this->handler)
        throw std::runtime_error("Error! Can't create circle button window");
}

HRESULT MyCircleButton::CreateGraphicsResources()
{
    HRESULT hResult = S_OK;

    if (SUCCEEDED(hResult))
    {
        /* window */
        RECT clientRect;
        GetClientRect(handler, &clientRect);

        D2D1_SIZE_U size = D2D1::SizeU(
            clientRect.right - clientRect.left,
            clientRect.bottom - clientRect.top
        );

        /* render */
        hResult = pAppFactory->CreateHwndRenderTarget(
            D2D1::RenderTargetProperties(
                D2D1_RENDER_TARGET_TYPE_DEFAULT,
                D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED)),
            D2D1::HwndRenderTargetProperties(handler, size),
            &pRenderTarget
        );

        if (SUCCEEDED(hResult))
        {
            hResult = pRenderTarget->CreateSolidColorBrush(fillColor, &pFillColorBrush);
        }
        if (SUCCEEDED(hResult))
        {
            hResult = pRenderTarget->CreateSolidColorBrush(borderColor, &pBorderColorBrush);
        }
        if (SUCCEEDED(hResult))
        {
            const float x = size.width / 2;
            const float y = size.height / 2;
            const float radius = min(x, y);
            ellipse = D2D1::Ellipse(D2D1::Point2F(x, y), radius, radius);
        }
    }

    return hResult;
}

/* Logic */

LRESULT CALLBACK MyCircleButton::CircleButtonProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
        case WM_PAINT:
            OnPaint();
            return 0;

        case WM_DESTROY:
            DiscardGraphicsResources();
            return 0;

        case WM_SIZE:
            Resize();
            return 0;

        default:
            return DefWindowProc(handler, uMsg, wParam, lParam);
    }
}

void MyCircleButton::OnPaint()
{
    HRESULT hResult = CreateGraphicsResources();
    if (SUCCEEDED(hResult))
    {
        /* prepare for drawing */
        PAINTSTRUCT paintStruct;
        BeginPaint(this->handler, &paintStruct);
        pRenderTarget->BeginDraw();
        pRenderTarget->Clear();

        /* draw window backgorund and border */
        pRenderTarget->FillEllipse(ellipse, pFillColorBrush);
        pRenderTarget->DrawEllipse(ellipse, pBorderColorBrush);

        /* end drawing*/
        hResult = pRenderTarget->EndDraw();
        if (FAILED(hResult) || hResult == D2DERR_RECREATE_TARGET)
        {
            DiscardGraphicsResources();
        }
        EndPaint(this->handler, &paintStruct);
    }
}

void MyCircleButton::Resize()
{
}

/* Finalization */

void MyCircleButton::DiscardGraphicsResources()
{
    SafeRelease(&pRenderTarget);
}

MyCircleButton::~MyCircleButton()
{
}