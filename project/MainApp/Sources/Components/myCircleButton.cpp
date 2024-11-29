#define _CRT_SECURE_NO_WARNINGS

#include "MainApp\Headers\App\General.h"
#include "MainApp\Headers\Components\MyCircleButton.h"

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

MyCircleButton::MyCircleButton(HWND parent, int ID, int posX, int posY, int radius, Gdiplus::Color fillColor)
{
    this->hParent = parent;
    this->menuID = ID;
    this->radius = radius;
    this->fillColor = fillColor;

    this->handler = CreateWindow(
        MyCircleButton::className, NULL,
        WS_CHILD | WS_VISIBLE,
        posX, posY, radius, radius,
        parent, NULL, NULL, this
    );

    if (!this->handler)
        throw std::runtime_error("Error! Can't create circle button window");
}

/* Logic */

LRESULT CALLBACK MyCircleButton::CircleButtonProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
        case WM_PAINT:
            OnPaint();
            return 0;

        case WM_LBUTTONDOWN:
            OnLBtnDown();
            return 0;

        default:
            return DefWindowProc(handler, uMsg, wParam, lParam);
    }
}

void MyCircleButton::OnPaint()
{
    /* prepare for drawing */
    PAINTSTRUCT paintStruct;
    HDC hDevContext = BeginPaint(this->handler, &paintStruct);

    RECT clientRect;
    GetClientRect(handler, &clientRect);

    Gdiplus::Graphics   graphics(hDevContext);
    Gdiplus::SolidBrush brush(fillColor);

    /* draw button */
    graphics.FillEllipse(&brush,
        static_cast<int>(clientRect.left),
        static_cast<int>(clientRect.top),
        static_cast<int>(clientRect.right - clientRect.left),
        static_cast<int>(clientRect.bottom - clientRect.top));

    /* end drawing*/
    EndPaint(this->handler, &paintStruct);
}

void MyCircleButton::OnLBtnDown()
{
    SendMessage(hParent, WM_COMMAND, MAKEWPARAM(menuID, BN_CLICKED), (LPARAM)handler);
}

/* Finalization */

MyCircleButton::~MyCircleButton()
{
}