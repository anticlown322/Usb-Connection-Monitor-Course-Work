#include "MainApp\Headers\App\General.h"
#include "MainApp\Headers\App\App.h"
#include "MainApp\Headers\Algs\SortAlgs.h"

#include "MainApp\Resources\resource.h"

/* prototypes for non class funcs */

void WatchRegistryKey(HKEY hKey, LPCWSTR subKey, HWND hMainApp);

/* Startup code */

MyApp::MyApp()
{
    try
    {
        this->InitWindow();
        this->CreateControls();
        this->InitMainList();
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

    this->wndScalingCoef_X = 0.6;
    this->wndScalingCoef_Y = 0.8;

    this->handler = CreateWindowEx(
        WS_EX_LAYERED, //for transparent d2d1 clear
        MyApp::className,
        MyApp::appName,
        WS_POPUP | WS_EX_TOPMOST, //borderless window
        (SCREEN_WIDTH * (1 - wndScalingCoef_X)) / 2,
        (SCREEN_HEIGHT * (1 - wndScalingCoef_Y)) / 2,
        SCREEN_WIDTH * wndScalingCoef_X,
        SCREEN_HEIGHT * wndScalingCoef_Y,
        nullptr,
        nullptr,
        nullptr,
        this);

    SetLayeredWindowAttributes( //for transparency
        this->handler,
        RGB(0, 0, 0),
        0,
        LWA_COLORKEY);

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
        darkRedColor);

    minimizeButton = new MyCircleButton(
        this->handler,
        (int)MENU_ID::MINIMIZE_BUTTON,
        clientRect.right - (TOP_MENU_BTN_RADIUS + TOP_MENU_BTN_GAP) * 2,
        TOP_MENU_BTN_Y,
        TOP_MENU_BTN_RADIUS,
        darkYellowColor);

    maximizeButton = new MyCircleButton(
        this->handler,
        (int)MENU_ID::MAXIMIZE_BUTTON,
        clientRect.right - (TOP_MENU_BTN_RADIUS + TOP_MENU_BTN_GAP) * 3,
        TOP_MENU_BTN_Y,
        TOP_MENU_BTN_RADIUS,
        darkGreenColor);

    /* main list */
    hMainList = CreateWindow(
        WC_LISTVIEW,
        L"",
        WS_CHILD | WS_VISIBLE | LVS_REPORT,
        LIST_MARGIN,
        HEADER_HEIGHT + LIST_MARGIN,
        clientRect.right - clientRect.left - LIST_MARGIN * 2,
        clientRect.bottom - clientRect.top - (HEADER_HEIGHT + LIST_MARGIN) * 2,
        handler,
        (HMENU)MENU_ID::MAIN_LIST,
        nullptr,
        nullptr);
}

void MyApp::InitMainList()
{
    RECT clientRect;
    GetClientRect(handler, &clientRect);

    /* init list columns */
    int colWidth = (clientRect.right - clientRect.top) / NUM_OF_COLS;
    LVCOLUMN column;
    column.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;

    for (int iCol = 0; iCol < NUM_OF_COLS; iCol++)
    {
        column.iSubItem = iCol;
        column.pszText = (wchar_t*)columnHeaders[iCol];
        column.cx = colWidth;

        SendMessage(hMainList, LVM_INSERTCOLUMN, (WPARAM)iCol, (LPARAM)&column);
    }

    /* add items to the list */
    this->deviceInfos = GetAllUsbDevices();
    ListView_DeleteAllItems(hMainList);
    FillMainList();
    UpdateColumnsWidths();
    InvalidateRect(handler, NULL, TRUE);
}

/* Logic */

//windows register

void WatchRegistryKey(HKEY hKey, LPCWSTR subKey, HWND hMainApp)
{
    HKEY key;
    HANDLE hChange;

    if (RegOpenKeyEx(hKey, subKey, 0, KEY_NOTIFY, &key) != ERROR_SUCCESS)
    {
        std::wcerr << L"Failed to open registry key." << std::endl;
        return;
    }

    hChange = CreateEvent(NULL, TRUE, FALSE, NULL);
    if (hChange == NULL)
    {
        std::wcerr << L"Failed to create event." << std::endl;
        RegCloseKey(key);
        return;
    }

    while (true)
    {
        if (RegNotifyChangeKeyValue(key, TRUE, REG_NOTIFY_CHANGE_NAME | REG_NOTIFY_CHANGE_LAST_SET, hChange, TRUE) != ERROR_SUCCESS)
        {
            std::wcerr << L"Failed to set registry notification." << std::endl;
            break;
        }

        DWORD result = WaitForSingleObject(hChange, 1000);

        if (result == WAIT_OBJECT_0)
        {
            SendMessage(hMainApp, WM_DEVICECHANGE, NULL, NULL);
        }
        else if (result == WAIT_TIMEOUT)
        {
            //do nothing or something
        }
        else
        {
            std::wcerr << L"Error in waiting for registry change." << std::endl;
            break;
        }
    }

    CloseHandle(hChange);
    RegCloseKey(key);
}

//main window

LRESULT MyApp::WndProc(UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
        case WM_CREATE:
            return OnCreate();

        case WM_DESTROY:
            OnDestroy();
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

        case WM_DEVICECHANGE:
            OnDeviceChange();
            return 0;

        case WM_NOTIFY:
            OnNotify(lParam);
            break;

        default:
            return DefWindowProc(handler, message, wParam, lParam);
    }
}

HRESULT MyApp::CreateGraphicsResources()
{
    HRESULT hResult = S_OK;

    if (NULL == pRenderTarget)
    {
        /* window */
        RECT clientRect;
        GetClientRect(handler, &clientRect);

        D2D1_SIZE_U size = D2D1::SizeU(
            clientRect.right - clientRect.left,
            clientRect.bottom - clientRect.top);

        /* render */
        hResult = pFactory->CreateHwndRenderTarget(
            D2D1::RenderTargetProperties(
                D2D1_RENDER_TARGET_TYPE_DEFAULT,
                D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED)),
            D2D1::HwndRenderTargetProperties(handler, size),
            &pRenderTarget);

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
                &pGradientStops);

            if (SUCCEEDED(hResult))
            {
                hResult = pRenderTarget->CreateLinearGradientBrush(
                    D2D1::LinearGradientBrushProperties(
                        D2D1::Point2F(clientRect.left, clientRect.top),
                        D2D1::Point2F(clientRect.right, clientRect.bottom)),
                    pGradientStops,
                    &pBackgroudnGradientBrush);
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
                &pGradientStops);

            if (SUCCEEDED(hResult))
            {
                hResult = pRenderTarget->CreateLinearGradientBrush(
                    D2D1::LinearGradientBrushProperties(
                        D2D1::Point2F(clientRect.left, clientRect.top),
                        D2D1::Point2F(clientRect.right, clientRect.bottom)),
                    pGradientStops,
                    &pWindowBorderBrush);
            }

            /* header text brush */
            pRenderTarget->CreateSolidColorBrush(
                D2D1::ColorF(D2D1::ColorF::White),
                &pHeaderTextBrush);
        }
    }

    return hResult;
}

int MyApp::OnCreate()
{
    if (FAILED(D2D1CreateFactory(
        D2D1_FACTORY_TYPE_SINGLE_THREADED,
        &pFactory)))
    {
        return -1;  // Fail CreateWindowEx.
    }

    if (FAILED(DWriteCreateFactory(
        DWRITE_FACTORY_TYPE_SHARED,
        __uuidof(IDWriteFactory),
        reinterpret_cast<IUnknown**>(&pDWriteFactory))))
    {
        return -1;
    }

    isMaximized = false;
    numOfDevices = 0;

    std::thread registryWatcher(WatchRegistryKey, HKEY_LOCAL_MACHINE, L"SYSTEM\\CurrentControlSet\\Enum", handler);
    registryWatcher.detach();

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

        D2D1_SIZE_F clientSizes = pRenderTarget->GetSize();

        /* draw window backgorund and border */
        if (isMaximized)
        {
            D2D1_RECT_F windowRect = D2D1::RectF(0, 0, clientSizes.width, clientSizes.height);
            pRenderTarget->FillRectangle(&windowRect, pBackgroudnGradientBrush);
            pRenderTarget->DrawRectangle(&windowRect, pWindowBorderBrush);
        }
        else
        {
            D2D1_ROUNDED_RECT roundedWindowRect = D2D1::RoundedRect(
                D2D1::RectF(0, 0, clientSizes.width, clientSizes.height),
                30.f,
                30.f
            );

            pRenderTarget->FillRoundedRectangle(&roundedWindowRect, pBackgroudnGradientBrush);
            pRenderTarget->DrawRoundedRectangle(&roundedWindowRect, pWindowBorderBrush, 2.0f, NULL);
        }

        /* draw text */
        //title text
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

        //bottom text
        std::wstring textToPrint = std::format(L"���������� ��������� USB-���������: {} ", numOfDevices).c_str();
        pDWriteFactory->CreateTextFormat(
            L"JetBrains Mono", nullptr,
            DWRITE_FONT_WEIGHT_BOLD,
            DWRITE_FONT_STYLE_NORMAL,
            DWRITE_FONT_STRETCH_MEDIUM,
            18.0f, L"en-US", &pTextFormat);

        pRenderTarget->DrawText(
            textToPrint.c_str(),
            textToPrint.length(),
            pTextFormat,
            D2D1::RectF(20, clientSizes.height - 18 - 20, clientSizes.width, clientSizes.height),
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
        /* preparing for resize */
        isMaximized = !isMaximized;

        RECT clientRect;
        GetClientRect(handler, &clientRect);
        D2D1_SIZE_U size = D2D1::SizeU(clientRect.right, clientRect.bottom);

        /* buttons positions */
        SetWindowPos(
            closeButton->handler,
            HWND_TOPMOST,
            clientRect.right - (TOP_MENU_BTN_RADIUS + TOP_MENU_BTN_GAP),
            TOP_MENU_BTN_Y,
            0,
            0,
            SWP_NOSIZE | SWP_NOZORDER);
        SetWindowPos(
            minimizeButton->handler,
            HWND_TOPMOST,
            clientRect.right - (TOP_MENU_BTN_RADIUS + TOP_MENU_BTN_GAP) * 2,
            TOP_MENU_BTN_Y,
            0,
            0,
            SWP_NOSIZE | SWP_NOZORDER);
        SetWindowPos(
            maximizeButton->handler,
            HWND_TOPMOST,
            clientRect.right - (TOP_MENU_BTN_RADIUS + TOP_MENU_BTN_GAP) * 3,
            TOP_MENU_BTN_Y,
            0,
            0,
            SWP_NOSIZE | SWP_NOZORDER);

        /* main list */
        SetWindowPos(
            hMainList,
            HWND_TOPMOST,
            LIST_MARGIN,
            HEADER_HEIGHT + LIST_MARGIN,
            clientRect.right - clientRect.left - LIST_MARGIN * 2,
            clientRect.bottom - clientRect.top - (HEADER_HEIGHT + LIST_MARGIN) * 2,
            SWP_NOZORDER);

        pRenderTarget->Resize(size);
        InvalidateRect(handler, NULL, TRUE);
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
                isMaximized ? ShowWindow(handler, SW_NORMAL) : ShowWindow(handler, SW_MAXIMIZE);
            }
            break;
    }
}

void MyApp::OnDestroy()
{
    DiscardGraphicsResources();
    DiscradControls();

    SafeRelease(&pFactory);
    SafeRelease(&pDWriteFactory);

    PostQuitMessage(0);
}

void MyApp::OnDeviceChange()
{
    deviceInfos = GetAllUsbDevices();
    ListView_DeleteAllItems(hMainList);
    FillMainList();
    UpdateColumnsWidths();
    InvalidateRect(handler, NULL, TRUE);
}

void MyApp::OnNotify(LPARAM lParam)
{
    LPNMHDR pnmh = (LPNMHDR)lParam;

    if (pnmh->hwndFrom == ListView_GetHeader(hMainList)) 
    {
        switch (pnmh->code) 
        {
            case HDN_ITEMCLICK: 
            {
                LPNMHEADER phdr = (LPNMHEADER)lParam;
                int clickedColumn = phdr->iItem;

                MergeSortMultiThreaded(deviceInfos, MAX_THREADS, (DEV_INFO)clickedColumn);
                ListView_DeleteAllItems(hMainList);
                FillMainList();
                UpdateColumnsWidths();
                InvalidateRect(handler, NULL, TRUE);
            }
            break;
        }
    }
}

//list

void MyApp::FillMainList()
{
    /* init items */
    numOfDevices = deviceInfos.size();
    int deviceIndex = 0;

    for (USBDeviceInfo device : deviceInfos)
    {
        const wchar_t** descriptions = new const wchar_t* [NUM_OF_COLS];

        //info
        descriptions[DEV_INFO::DESC] = device.description.c_str();
        descriptions[DEV_INFO::MFG] = device.manufactoring.c_str();
        descriptions[DEV_INFO::SERVICE] = device.serviceName.c_str();

        //status
        descriptions[DEV_INFO::IS_CONNECTED] = device.isConnected ? L"��" : L"���";
        descriptions[DEV_INFO::IS_DIASBLED] = device.isDisabled ? L"��" : L"���";
        descriptions[DEV_INFO::IS_SAFE_TO_UNPLUG] = device.isSafeToUnplug ? L"��" : L"���";

        //capabilities
        descriptions[DEV_INFO::IS_USB_HUB] = device.isUsbHub ? L"��" : L"���";
        descriptions[DEV_INFO::CAPABILITIES] = device.capabilities.c_str();

        //location
        descriptions[DEV_INFO::LOCATION] = device.location.c_str();

        //addintional info
        descriptions[DEV_INFO::SERIAL_NUM] = device.serialNumber.c_str();
        descriptions[DEV_INFO::USB_CLASS] = device.usbClass.c_str();
        descriptions[DEV_INFO::USB_SUBCLASS] = device.usbSubclass.c_str();
        descriptions[DEV_INFO::USB_PROTOCOL] = device.usbProtocol.c_str();

        //ids
        descriptions[DEV_INFO::ID] = device.id.c_str();
        descriptions[DEV_INFO::DEV_GUID] = device.devClassGUID.c_str();
        descriptions[DEV_INFO::HARDWARE_ID] = device.hardwareID.c_str();
        descriptions[DEV_INFO::PARENT_PREFIX] = device.parentIDPrefix.c_str();

        InsertItemWithSubItems(deviceIndex, descriptions, NUM_OF_COLS);
        deviceIndex++;
    }
}

void MyApp::SetSubItems(int itemIndex, const wchar_t** texts, int count) {
    LVITEM item;
    item.mask = LVIF_TEXT;
    item.iItem = itemIndex;

    for (int i = 1; i < count; i++) { // �������� � 1, ��� ��� 0 ��� ���������� ��� ������� ��������
        item.iSubItem = i;
        item.pszText = (wchar_t*)texts[i];
        ListView_SetItem(hMainList, &item);
    }
}

void MyApp::InsertItemWithSubItems(int itemIndex, const wchar_t** texts, int count) {
    LVITEM item;
    item.mask = LVIF_TEXT;
    item.iItem = itemIndex;
    item.iSubItem = 0;
    item.pszText = (wchar_t*)texts[0];

    int newItemIndex = ListView_InsertItem(hMainList, &item);

    if (newItemIndex != -1) {
        SetSubItems(newItemIndex, texts, count);
    }
    else {
        MessageBox(
            NULL,
            L"Failed to insert item into ListView",
            L"Error",
            MB_OK | MB_ICONERROR);
    }
}

void MyApp::UpdateColumnsWidths()
{
    for (int iCol = 0; iCol < NUM_OF_COLS; iCol++)
    {
        int maxWidth = 0;
        int textWidth = 0;

        if (DEV_INFO::IS_CONNECTED == iCol ||
            DEV_INFO::IS_DIASBLED == iCol ||
            DEV_INFO::IS_SAFE_TO_UNPLUG == iCol ||
            DEV_INFO::IS_USB_HUB == iCol ||
            DEV_INFO::USB_CLASS == iCol ||
            DEV_INFO::USB_SUBCLASS == iCol ||
            DEV_INFO::USB_PROTOCOL == iCol)
        {
            maxWidth = 30;
        }
        else
            for (USBDeviceInfo device : deviceInfos)
            {
                switch (iCol)
                {
                    case DEV_INFO::DESC: textWidth = ListView_GetStringWidth(hMainList, device.description.c_str()); break;
                    case DEV_INFO::MFG: textWidth = ListView_GetStringWidth(hMainList, device.manufactoring.c_str()); break;
                    case DEV_INFO::SERVICE: textWidth = ListView_GetStringWidth(hMainList, device.serviceName.c_str()); break;

                    case DEV_INFO::CAPABILITIES: textWidth = ListView_GetStringWidth(hMainList, device.capabilities.c_str()); break;
                
                    case DEV_INFO::LOCATION: textWidth = ListView_GetStringWidth(hMainList, device.location.c_str()); break;

                    case DEV_INFO::SERIAL_NUM: textWidth = ListView_GetStringWidth(hMainList, device.serialNumber.c_str()); break;

                    case DEV_INFO::ID: textWidth = ListView_GetStringWidth(hMainList, device.id.c_str()); break;
                    case DEV_INFO::DEV_GUID: textWidth = ListView_GetStringWidth(hMainList, device.devClassGUID.c_str()); break;
                    case DEV_INFO::HARDWARE_ID: textWidth = ListView_GetStringWidth(hMainList, device.hardwareID.c_str()); break;
                    case DEV_INFO::PARENT_PREFIX: textWidth = ListView_GetStringWidth(hMainList, device.parentIDPrefix.c_str()); break;
                }

                if (textWidth > maxWidth)
                    maxWidth = textWidth;
            }

        int columnHeaderLen = ListView_GetStringWidth(hMainList, columnHeaders[iCol]);
        if (columnHeaderLen > maxWidth)
            maxWidth = columnHeaderLen + 20;

        ListView_SetColumnWidth(hMainList, iCol, maxWidth + 20);
    }
}

/* Finalization */

MyApp::~MyApp()
{
}

void MyApp::DiscradControls()
{
    if (closeButton)
        delete closeButton;

    if (minimizeButton)
        delete minimizeButton;

    if (maximizeButton)
        delete maximizeButton;
}

void MyApp::DiscardGraphicsResources()
{
    SafeRelease(&pHeaderTextBrush);
    SafeRelease(&pBackgroudnGradientBrush);
    SafeRelease(&pWindowBorderBrush);
    SafeRelease(&pRenderTarget);
}