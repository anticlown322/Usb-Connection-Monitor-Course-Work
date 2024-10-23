#ifndef MY_APP_CLASS
#define MY_APP_CLASS

#include "myCircleButton.h"

class MyApp
{
public:
    /* funcs */
    explicit MyApp();
    ~MyApp();
    int Run();

    /* static funcs */
    static LRESULT CALLBACK AppProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

    /* static fields */
    static constexpr const wchar_t* appName = L"File Sorter";
    static constexpr const wchar_t* className = L"MyAppWindowClass";

private:
    /* funcs */
    LRESULT CALLBACK WndProc(UINT message, WPARAM wParam, LPARAM lParam);

    //initialization
    void    InitWindow();
    void    CreateControls();
    HRESULT CreateGraphicsResources();

    //logic
    void    OnPaint();
    void    OnSize();
    void    OnLBtnDown();
    void    OnCommand(WPARAM menuId);

    //finalization
    void    DiscradControls();
    void    DiscardGraphicsResources();

    /* static funcs */

    /* fields */
    HWND handler;

    //d2d1
    ID2D1Factory* pFactory;
    ID2D1HwndRenderTarget* pRenderTarget;
    ID2D1LinearGradientBrush* pBackgroudnGradientBrush;
    ID2D1LinearGradientBrush* pWindowBorderBrush;

    //gdi
    Gdiplus::GdiplusStartupInput gdiplusStartupInput;
    ULONG_PTR gdiplusToken;

    //objects
    MyCircleButton* closeButton;
    MyCircleButton* minimizeButton;
    MyCircleButton* maximizeButton;

    /* static fields */
    static const int startWindowWidth = 800;
    static const int startWindowHeight = 600;

    static const int TOP_MENU_BTN_RADIUS = 20;
    static const int TOP_MENU_BTN_GAP = 10;
    static const int TOP_MENU_BTN_Y = 10;
};

#endif // MY_APP_CLASS

