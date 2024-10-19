#ifndef MY_APP_CLASS
#define MY_APP_CLASS

class MainWindow : public BaseWindow<MainWindow>
{
private:
    /* fields */
    ID2D1Factory* pFactory;
    ID2D1HwndRenderTarget* pRenderTarget;

    ID2D1LinearGradientBrush* pBackgroudnGradientBrush;
    ID2D1LinearGradientBrush* pWindowBorderBrush;

    /* funcs */
    HRESULT CreateGraphicsResources();
    void    DiscardGraphicsResources();
    void    OnPaint();
    void    Resize();

public:
    /* fields*/
    PCWSTR  ClassName() const { return L"USB Monitor window Class"; }
    LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);

    /* funcs */
    MainWindow();

    /* static */
    static const int startWndWidth = 800;
    static const int startWndHeight = 600;
};

#endif // MY_APP_CLASS

