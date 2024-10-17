#include "general.h"
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
    windowClass.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
    windowClass.hInstance = GetModuleHandle(nullptr);
    windowClass.lpfnWndProc = MyApp::AppProc;
    windowClass.lpszClassName = MyApp::className;
    windowClass.lpszMenuName = nullptr;
    windowClass.style = CS_VREDRAW | CS_HREDRAW;

    /* register classes */
    if (!RegisterClass(&windowClass)) // window
        throw std::runtime_error("Error! Can't register main window class");

    /* changing client area */
    RECT windowRect{
        0,
        0,
        this->startWindowWidth,
        this->startWindowHeight
    };
    AdjustWindowRect(
        &windowRect,
        WS_OVERLAPPEDWINDOW,
        false
    );

    /* creating main window */
    this->handler = CreateWindow(
        MyApp::className,
        MyApp::appName,
        WS_OVERLAPPEDWINDOW,
        (SCREEN_WIDTH - windowRect.right) / 2,
        (SCREEN_HEIGHT - windowRect.bottom) / 2,
        windowRect.right,
        windowRect.bottom,
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
    switch (message)
    {
        case WM_CREATE:
            {

            }
            return 0;

        case WM_PAINT:
            {
                PAINTSTRUCT ps;
                HDC hdc = BeginPaint(hwnd, &ps);

                FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));

                EndPaint(hwnd, &ps);
            }
            return 0;

        case WM_SIZE:
            {
                RECT clientArea;
                GetClientRect(hwnd, &clientArea);

                
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
