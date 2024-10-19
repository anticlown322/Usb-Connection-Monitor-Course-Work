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
};

/* Utility */
#include <string>

/* UI */
#define SCREEN_WIDTH  GetSystemMetrics(SM_CXSCREEN)
#define SCREEN_HEIGHT GetSystemMetrics(SM_CYSCREEN)

#include <Windows.h>
#include <wingdi.h>
//#include <windowsx.h>
//#include <CommCtrl.h>

#endif // GENERAL_H