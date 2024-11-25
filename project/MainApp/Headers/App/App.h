#ifndef MY_APP_CLASS
#define MY_APP_CLASS

#include "MainApp\Headers\Components\myCircleButton.h"

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

    //initialization
    void InitWindow();
    void CreateControls();
    void InitMainList();

    //logic
    LRESULT CALLBACK WndProc(UINT message, WPARAM wParam, LPARAM lParam);
    HRESULT CreateGraphicsResources();

    int  OnCreate();
    void OnPaint();
    void OnSize();
    void OnLBtnDown();
    void OnCommand(WPARAM menuId);
    void OnDestroy();

    //list
    void SetSubItems(int itemIndex, const wchar_t** texts, int count);
    void InsertItemWithSubItems(int itemIndex, const wchar_t** texts, int count);

    //finalization
    void DiscradControls();
    void DiscardGraphicsResources();

    /* static funcs */

    /* fields */
    HWND handler;
    HFONT hHeaderFont;
    int numOfDevices;

    //d2d1
    ID2D1Factory*             pFactory;
    IDWriteFactory*           pDWriteFactory;
    ID2D1HwndRenderTarget*    pRenderTarget;
    ID2D1LinearGradientBrush* pBackgroudnGradientBrush;
    ID2D1LinearGradientBrush* pWindowBorderBrush;
    ID2D1SolidColorBrush*     pHeaderTextBrush;

    //gdi
    Gdiplus::GdiplusStartupInput gdiplusStartupInput;
    ULONG_PTR                    gdiplusToken;

    //objects
    MyCircleButton* closeButton;
    MyCircleButton* minimizeButton;
    MyCircleButton* maximizeButton;
    HWND hMainList;

    //utility
    double wndScalingCoef_X;
    double wndScalingCoef_Y;
    bool   isMaximized;

    /* static fields */
    static const int TOP_MENU_BTN_RADIUS = 20;
    static const int TOP_MENU_BTN_GAP    = 10;
    static const int TOP_MENU_BTN_Y      = 10;
    static const int HEADER_HEIGHT       = 50;

    static const int LIST_MARGIN = 1;
    static const int NUM_OF_COLS = 19;
};

#endif // MY_APP_CLASS

