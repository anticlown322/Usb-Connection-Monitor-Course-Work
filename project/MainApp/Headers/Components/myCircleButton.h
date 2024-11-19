#ifndef MY_CIRCLE_BUTTON_H
#define MY_CIRCLE_BUTTON_H

class MyCircleButton
{
public:
    /* funcs */
    MyCircleButton(HWND parent, int ID,
                   int posX, int posY, int radius, Gdiplus::Color fillColor);
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
    void OnPaint();
    void OnLBtnDown();

    /* fields */
    int radius;
    Gdiplus::Color fillColor;
};

#endif // MY_CIRCLE_BUTTON_H
