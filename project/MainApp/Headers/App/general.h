#ifndef GENERAL_H
#define GENERAL_H

/* App */
#include <stdlib.h>
#include <exception>
#include <fstream>

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

#endif // GENERAL_H