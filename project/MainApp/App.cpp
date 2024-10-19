#include "general.h"
#include "resource.h"
#include "App.h"

MyApp::MyApp()
{
    try
    {
        this->InitWindow();
        this->CreateControls();
    }
    catch (const std::exception& exception)
    {
        std::string exceptionData = exception.what();
        MessageBox(
            nullptr,
            std::wstring(begin(exceptionData), end(exceptionData)).c_str(),
            L"Error!",
            MB_ICONERROR | MB_OK
        );
        ExitProcess(EXIT_FAILURE);
    }
}

void MyApp::InitWindow()
{
    /* init main window */
    WNDCLASS windowClass = { 0 };
    windowClass.cbClsExtra = 0;
    windowClass.cbWndExtra = 0;
    windowClass.hbrBackground = reinterpret_cast<HBRUSH>(GetStockObject(WHITE_BRUSH));
    windowClass.hCursor = LoadCursor(nullptr, IDC_ARROW);
    windowClass.hInstance = GetModuleHandle(nullptr);
    windowClass.hIcon = LoadIcon(windowClass.hInstance, MAKEINTRESOURCE(IDI_ICON1));
    windowClass.lpfnWndProc = MyApp::AppProc;
    windowClass.lpszClassName = MyApp::className;
    windowClass.lpszMenuName = nullptr;
    windowClass.style = CS_VREDRAW | CS_HREDRAW;

    /* GDI+ initialization*/
    Gdiplus::GdiplusStartup(&this->gdiplusToken, &this->gdiplusStartupInput, NULL);

    /* register classes */
    if (!RegisterClass(&windowClass)) // window
        throw std::runtime_error("Error! Can't register main window class");

    /* creating main window */
    this->handler = CreateWindow(
        MyApp::className,
        MyApp::appName,
        WS_POPUP | WS_VISIBLE, //borderless window
        (SCREEN_WIDTH - this->startWindowWidth) / 2,
        (SCREEN_HEIGHT - this->startWindowHeight) / 2,
        this->startWindowWidth,
        this->startWindowHeight,
        nullptr,
        nullptr,
        nullptr,
        this
    );

    if (!this->handler)
        throw std::runtime_error("Error! Can't create main window");
}

void MyApp::CreateControls()
{

}

int MyApp::Run()
{
    ShowWindow(this->handler, SW_SHOWDEFAULT);
    UpdateWindow(this->handler);

    MSG message{};
    while (GetMessage(&message, nullptr, 0, 0))
    {
        TranslateMessage(&message);
        DispatchMessage(&message);
    }

    Gdiplus::GdiplusShutdown(gdiplusToken);//free gdi+ object
    return static_cast<int>(message.wParam);
}

MyApp::~MyApp()
{
}

LRESULT MyApp::AppProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    MyApp* pApp;

    /* get pApp value */
    if (message == WM_NCCREATE)
    {
        pApp = static_cast<MyApp*>(reinterpret_cast<CREATESTRUCT*>(lParam)->lpCreateParams);
        SetLastError(0);

        if (!SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pApp)))
        {
            if (GetLastError() != 0)
                return false;
        }
    }
    else
    {
        pApp = reinterpret_cast<MyApp*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
    }

    /* process message  */
    if (pApp)
    {
        pApp->handler = hwnd;
        return pApp->WndProc(hwnd, message, wParam, lParam);
    }

    return DefWindowProc(hwnd, message, wParam, lParam);
}

LRESULT MyApp::WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    RECT clientArea;
    GetClientRect(hwnd, &clientArea);

    switch (message)
    {
        case WM_CREATE:
            {

            }
            return 0;

        case WM_PAINT:
            {
                PAINTSTRUCT paintStruct;
                HDC hDevContext = BeginPaint(hwnd, &paintStruct);
                Gdiplus::Graphics graphics(hDevContext);

                /* paint gradient background */
                Gdiplus::LinearGradientBrush linGrBrush(
                    Gdiplus::Point(static_cast<int>(clientArea.left), 
                                   static_cast<int>(clientArea.top)),
                    Gdiplus::Point(static_cast<int>(clientArea.right - clientArea.left), 
                                   static_cast<int>(clientArea.bottom - clientArea.top)),
                    Gdiplus::Color(255, 255, 0, 0), // Opaque red
                    Gdiplus::Color(255, 0, 0, 255) // Opaque blue
                );  
                
                linGrBrush.SetGammaCorrection(TRUE);
                graphics.FillRectangle(
                    &linGrBrush, 
                    static_cast<int>(clientArea.left),
                    static_cast<int>(clientArea.top),
                    static_cast<int>(clientArea.right - clientArea.left),
                    static_cast<int>(clientArea.bottom - clientArea.top)
                );
                
                EndPaint(hwnd, &paintStruct);
            }
            return 0;

        case WM_SIZE:
            {

            }
            return 0;

        case WM_KEYDOWN:
            return 0;

        case WM_COMMAND:
            switch (LOWORD(wParam))
            {

            }
            return 0;

        case WM_CLOSE:
            {
                DestroyWindow(hwnd);
            }
            return 0;

        case WM_DESTROY:
            {
                PostQuitMessage(0);
            }
            return 0;

        default:
            return DefWindowProc(hwnd, message, wParam, lParam);
    }
}
