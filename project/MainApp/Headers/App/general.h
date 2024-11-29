#ifndef GENERAL_H
#define GENERAL_H

/* App */
#include <stdlib.h>
#include <exception>
#include <iostream>

#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif

#ifndef UNICODE
#define UNICODE
#endif

enum class MENU_ID {
    CLOSE_BUTTON = 10,
    MINIMIZE_BUTTON,
    MAXIMIZE_BUTTON,

    MAIN_LIST = 20
};

/* Utility */
#include <string>
#include <format>
#include <sstream> 
#include <vector>
#include <regex>

template <class T> void SafeRelease(T** ppT)
{
    if (*ppT)
    {
        (*ppT)->Release();
        *ppT = NULL;
    }
}

/* UI */
#define SCREEN_WIDTH  GetSystemMetrics(SM_CXSCREEN)
#define SCREEN_HEIGHT GetSystemMetrics(SM_CYSCREEN)

#include <windows.h>
#include <commctrl.h>
#include <d2d1.h>
#include <dwrite.h>
#include <gdiplus.h>

/* Algs */
#include <initguid.h> 
#include <devguid.h> 
#include <usbiodef.h>
#include <devpkey.h>
#include <setupapi.h> 
#include <cfgmgr32.h> 
#include <thread>

enum DEV_INFO : int {
    DESC,
    MFG,
    SERVICE,

    IS_CONNECTED,
    IS_DIASBLED,
    IS_SAFE_TO_UNPLUG,
    IS_USB_HUB,
    CAPABILITIES,

    LOCATION,

    SERIAL_NUM,
    USB_CLASS,
    USB_SUBCLASS,
    USB_PROTOCOL,

    ID,
    DEV_GUID,
    HARDWARE_ID,
    PARENT_PREFIX
};

struct USBDeviceInfo {
    std::wstring description;
    std::wstring manufactoring;
    std::wstring serviceName;

    bool isConnected;
    bool isDisabled;
    bool isSafeToUnplug;
    bool isUsbHub;
    std::wstring capabilities;

    std::wstring location;

    std::wstring serialNumber;
    std::wstring usbClass;
    std::wstring usbSubclass;
    std::wstring usbProtocol;

    std::wstring id;
    std::wstring devClassGUID;
    std::wstring hardwareID;
    std::wstring parentIDPrefix;
};

#endif // GENERAL_H