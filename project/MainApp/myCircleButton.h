#ifndef MY_CIRCLE_BUTTON_H
#define MY_CIRCLE_BUTTON_H

class MyCircleButton
{
public:
    /* funcs */
    MyCircleButton(HWND parent, ID2D1Factory* AppFactory,
        int ID, int posX, int posY, int radius, D2D1_COLOR_F fillColor, D2D1_COLOR_F borderColor);
    ~MyCircleButton();
    static bool RegisterCustomButtonClass(HINSTANCE hInstance);
    static LRESULT CALLBACK StaticCircleButtonProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
    LRESULT CALLBACK CircleButtonProc(UINT message, WPARAM wParam, LPARAM lParam);

    /* fields */
    HWND handler;
    HWND hParent;
    int  menuID;

    /* static fields */
    static constexpr const wchar_t* className = L"CustomButtonClass";
private:
    /* funcs */
    HRESULT CreateGraphicsResources();
    void    DiscardGraphicsResources();
    void    OnPaint();
    void    Resize();

    /* fields */
    int                    radius;

    ID2D1Factory*          pAppFactory;
    ID2D1HwndRenderTarget* pRenderTarget;
    D2D1_COLOR_F           fillColor;
    D2D1_COLOR_F           borderColor;
    ID2D1SolidColorBrush*  pFillColorBrush;
    ID2D1SolidColorBrush*  pBorderColorBrush;
    D2D1_ELLIPSE           ellipse;

    /* static fields */
    static const int DEFAULT_RADIUS = 30;
};

#endif // MY_CIRCLE_BUTTON_H
