#include "MainApp\Headers\App\general.h"
#include "MainApp\Headers\App\App.h"

#include "MainApp\Resources\resource.h"

/* Startup code */

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

LRESULT MyApp::AppProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    MyApp* pThis = NULL;

    if (message == WM_NCCREATE)
    {
        CREATESTRUCT* pCreate = (CREATESTRUCT*)lParam;
        pThis = (MyApp*)pCreate->lpCreateParams;
        SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)pThis);

        pThis->handler = hwnd;
    }
    else
    {
        pThis = (MyApp*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
    }

    if (pThis)
    {
        return pThis->WndProc(message, wParam, lParam);
    }
    else
    {
        return DefWindowProc(hwnd, message, wParam, lParam);
    }
}

/* Initialization */

void MyApp::InitWindow()
{
    /* GDI+ initialization*/
    Gdiplus::GdiplusStartup(&this->gdiplusToken, &this->gdiplusStartupInput, NULL);

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

    /* register classes */
    if (!RegisterClass(&windowClass)) // window
        throw std::runtime_error("Error! Can't register main window class");
    if (!MyCircleButton::RegisterCustomButtonClass(windowClass.hInstance)) // circle button
        throw std::runtime_error("Error! Can't register circle button class");

    /* creating main window */
    this->handler = CreateWindowEx(
        WS_EX_LAYERED, //for transparent d2d1 clear
        MyApp::className,
        MyApp::appName,
        WS_POPUP | WS_EX_TOPMOST, //borderless window
        (SCREEN_WIDTH - this->startWindowWidth) / 2,
        (SCREEN_HEIGHT - this->startWindowHeight) / 2,
        this->startWindowWidth,
        this->startWindowHeight,
        nullptr,
        nullptr,
        nullptr,
        this
    );

    SetLayeredWindowAttributes(this->handler, RGB(0, 0, 0), 0, LWA_COLORKEY); //for transparency

    if (!this->handler)
        throw std::runtime_error("Error! Can't create main window");
}

void MyApp::CreateControls()
{
    RECT clientRect;
    GetClientRect(handler, &clientRect);

    /* top menu buttons */
    Gdiplus::Color darkRedColor(245, 66, 108);
    Gdiplus::Color darkYellowColor(217, 201, 63);
    Gdiplus::Color darkGreenColor(107, 217, 52);

    closeButton = new MyCircleButton(
        this->handler,
        (int)MENU_ID::CLOSE_BUTTON,
        clientRect.right - (TOP_MENU_BTN_RADIUS + TOP_MENU_BTN_GAP),
        TOP_MENU_BTN_Y,
        TOP_MENU_BTN_RADIUS,
        darkRedColor
    );

    minimizeButton = new MyCircleButton(
        this->handler,
        (int)MENU_ID::MINIMIZE_BUTTON,
        clientRect.right - (TOP_MENU_BTN_RADIUS + TOP_MENU_BTN_GAP) * 2,
        TOP_MENU_BTN_Y,
        TOP_MENU_BTN_RADIUS,
        darkYellowColor
    );

    new MyCircleButton(
        this->handler,
        (int)MENU_ID::MAXIMIZE_BUTTON,
        clientRect.right - (TOP_MENU_BTN_RADIUS + TOP_MENU_BTN_GAP) * 3,
        TOP_MENU_BTN_Y,
        TOP_MENU_BTN_RADIUS,
        darkGreenColor
    );
}

void MyApp::DiscradControls()
{
    if (closeButton)
        delete closeButton;

    if (minimizeButton)
        delete minimizeButton;
}

HRESULT MyApp::CreateGraphicsResources()
{
    HRESULT hResult = S_OK;

    if (pRenderTarget == NULL)
    {
        /* window */
        RECT clientRect;
        GetClientRect(handler, &clientRect);

        D2D1_SIZE_U size = D2D1::SizeU(
            clientRect.right - clientRect.left,
            clientRect.bottom - clientRect.top
        );

        /* render */
        hResult = pFactory->CreateHwndRenderTarget(
            D2D1::RenderTargetProperties(
                D2D1_RENDER_TARGET_TYPE_DEFAULT,
                D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED)),
            D2D1::HwndRenderTargetProperties(handler, size),
            &pRenderTarget
        );

        if (SUCCEEDED(hResult))
        {
            /* background  brush */
            ID2D1GradientStopCollection* pGradientStops = NULL;
            D2D1_GRADIENT_STOP gradientStops[2];
            gradientStops[0].color = D2D1::ColorF(D2D1::ColorF::CornflowerBlue, 1);
            gradientStops[0].position = 0.0f;
            gradientStops[1].color = D2D1::ColorF(D2D1::ColorF::DodgerBlue, 1);
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
                    &pWindowBorderBrush
                );
            }

            /* header text brush */
            pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::White), &pHeaderTextBrush);
        }
    }

    return hResult;
}

/* Logic */

int MyApp::OnCreate()
{
    if (FAILED(D2D1CreateFactory(
        D2D1_FACTORY_TYPE_SINGLE_THREADED, &pFactory)))
    {
        return -1;  // Fail CreateWindowEx.
    }

    if (FAILED(DWriteCreateFactory(
        DWRITE_FACTORY_TYPE_SHARED,
        __uuidof(IDWriteFactory),
        reinterpret_cast<IUnknown**>(&pDWriteFactory)
    )))
    {
        return -1;
    }

    return 0;
}

void MyApp::OnPaint()
{
    HRESULT hResult = CreateGraphicsResources();

    if (SUCCEEDED(hResult))

    {
        /* prepare for drawing */
        PAINTSTRUCT paintStruct;
        BeginPaint(this->handler, &paintStruct);
        HDC hDevContext = GetWindowDC(handler);

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

        /* draw text */
        IDWriteTextFormat* pTextFormat = nullptr;
        pDWriteFactory->CreateTextFormat(
            L"JetBrains Mono", nullptr, 
            DWRITE_FONT_WEIGHT_BOLD, 
            DWRITE_FONT_STYLE_NORMAL, 
            DWRITE_FONT_STRETCH_MEDIUM,
            24.0f, L"en-US", &pTextFormat);

        pRenderTarget->DrawText(
            L"USB Monitor", 
            wcslen(L"USB Monitor"),
            pTextFormat, 
            D2D1::RectF(20, 10, clientSizes.width, clientSizes.height),
            pHeaderTextBrush);

        pTextFormat->Release();

        /* end drawing*/
        hResult = pRenderTarget->EndDraw();
        if (FAILED(hResult) || hResult == D2DERR_RECREATE_TARGET)
        {
            DiscardGraphicsResources();
        }
        EndPaint(this->handler, &paintStruct);
    }
}

void MyApp::OnSize()
{
    if (pRenderTarget != NULL)
    {
        RECT clientRect;
        GetClientRect(handler, &clientRect);

        D2D1_SIZE_U size = D2D1::SizeU(clientRect.right, clientRect.bottom);

        pRenderTarget->Resize(size);
        InvalidateRect(handler, NULL, FALSE);
    }
}

void MyApp::OnLBtnDown()
{
    SendMessage(handler, WM_SYSCOMMAND, SC_MOVE | HTCAPTION, 0);
}

void MyApp::OnCommand(WPARAM menuId)
{
    switch (LOWORD(menuId))
    {
        case (int)MENU_ID::CLOSE_BUTTON:
            {
                DestroyWindow(handler);
            }
            break;

        case (int)MENU_ID::MINIMIZE_BUTTON:
            {
                ShowWindow(handler, SW_MINIMIZE);
            }
            break;

        case (int)MENU_ID::MAXIMIZE_BUTTON:
            {
                ShowWindow(handler, SW_MAXIMIZE);
            }
            break;
    }
}

LRESULT MyApp::WndProc(UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
        case WM_CREATE:
            return OnCreate();

        case WM_DESTROY:
            DiscardGraphicsResources();
            DiscradControls();
            SafeRelease(&pFactory);
            SafeRelease(&pDWriteFactory);
            PostQuitMessage(0);
            return 0;

        case WM_PAINT:
            OnPaint();
            return 0;

        case WM_SIZE:
            OnSize();
            return 0;

        case WM_LBUTTONDOWN:
            OnLBtnDown();
            return 0;

        case WM_COMMAND:
            OnCommand(wParam);
            return 0;

        default:
            return DefWindowProc(handler, message, wParam, lParam);
    }
}

/* Finalization */

MyApp::~MyApp()
{
}

void MyApp::DiscardGraphicsResources()
{
    SafeRelease(&pHeaderTextBrush);
    SafeRelease(&pBackgroudnGradientBrush);
    SafeRelease(&pWindowBorderBrush);
    SafeRelease(&pRenderTarget);
}