// Brightness.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "Brightness.h"

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                // current instance
TCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

#define MAX_MONITOR_COUNT 16
size_t MonitorCount = 0;
HANDLE Monitor[MAX_MONITOR_COUNT];

BOOL __stdcall EnumFunction(HMONITOR LogicalMonitor, HDC, LPRECT rc, LPARAM)
{
    DWORD Number;
    if (!GetNumberOfPhysicalMonitorsFromHMONITOR(LogicalMonitor, &Number))
    {
        return TRUE;
    }

    PHYSICAL_MONITOR* Monitors = (PHYSICAL_MONITOR*)malloc(sizeof(PHYSICAL_MONITOR)*Number);
    if (!Monitors)
    {
        return TRUE;
    }

    if (!GetPhysicalMonitorsFromHMONITOR(LogicalMonitor, Number, Monitors))
    {
        free(Monitors);
        return TRUE;
    }

    for (size_t i = 0; i < Number; i++)
    {
        Monitor[MonitorCount] = Monitors[i].hPhysicalMonitor;
        MonitorCount++;
        if (MonitorCount > MAX_MONITOR_COUNT) return FALSE;
    }
    return TRUE;
}


int APIENTRY _tWinMain(HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPTSTR    lpCmdLine,
    int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    MSG msg;

    EnumDisplayMonitors(NULL, NULL, EnumFunction, 0);

    // Initialize global strings
    LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadString(hInstance, IDC_BRIGHTNESS, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance(hInstance, nCmdShow))
    {
        return FALSE;
    }

    // Main message loop:
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return (int)msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
//  COMMENTS:
//
//    This function and its usage are only necessary if you want this code
//    to be compatible with Win32 systems prior to the 'RegisterClassEx'
//    function that was added to Windows 95. It is important to call this function
//    so that the application will get 'well formed' small icons associated
//    with it.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEX wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_BRIGHTNESS));
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = MAKEINTRESOURCE(IDC_BRIGHTNESS);
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassEx(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    HWND hWnd;

    hInst = hInstance; // Store instance handle in our global variable

    hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

    if (!hWnd)
    {
        return FALSE;
    }

    ShowWindow(hWnd, SW_HIDE);
    UpdateWindow(hWnd);

    return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    int wmId, wmEvent;
    PAINTSTRUCT ps;
    HDC hdc;

    switch (message)
    {
    case WM_CREATE: {
        for (int i = 0; i < 9; i++)
        {
            char Key = '1' + i;
            RegisterHotKey(hWnd, i, MOD_ALT | MOD_CONTROL, Key);
        }

        DEV_BROADCAST_DEVICEINTERFACE notificationFilter;
        ZeroMemory(&notificationFilter, sizeof(notificationFilter));

        notificationFilter.dbcc_size = sizeof(notificationFilter);
        notificationFilter.dbcc_devicetype = DBT_DEVTYP_DEVICEINTERFACE;
        notificationFilter.dbcc_reserved = 0;

        notificationFilter.dbcc_classguid = GUID_DEVINTERFACE_MONITOR;
        RegisterDeviceNotification(hWnd, &notificationFilter, DEVICE_NOTIFY_WINDOW_HANDLE);
        break;
    }
    case WM_HOTKEY:
        if (wParam <= 9)
        {
            for (size_t i = 0; i < MonitorCount; i++)
            {
                if (wParam == 8)
                {
                    SetMonitorContrast(Monitor[i], 50);
                    SetMonitorBrightness(Monitor[i], 80);
                }
                else
                {
                    SetMonitorBrightness(Monitor[i], wParam * 60 / 7);
                    SetMonitorContrast(Monitor[i], 30);
                }
            }
        }
        break;

    case WM_COMMAND:
        wmId = LOWORD(wParam);
        wmEvent = HIWORD(wParam);
        // Parse the menu selections:
        switch (wmId)
        {
        case IDM_EXIT:
            DestroyWindow(hWnd);
            break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
        break;
    case WM_PAINT:
        hdc = BeginPaint(hWnd, &ps);
        // TODO: Add any drawing code here...
        EndPaint(hWnd, &ps);
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    case WM_DEVICECHANGE:
    {

        if (wParam != DBT_DEVICEARRIVAL && wParam != DBT_DEVICEREMOVECOMPLETE) break;

        PDEV_BROADCAST_HDR lpdb = (PDEV_BROADCAST_HDR)lParam;
        if (lpdb->dbch_devicetype != DBT_DEVTYP_DEVICEINTERFACE) break;

        PDEV_BROADCAST_DEVICEINTERFACE devInterface = (PDEV_BROADCAST_DEVICEINTERFACE)lpdb;
        if (!InlineIsEqualGUID(devInterface->dbcc_classguid, GUID_DEVINTERFACE_MONITOR)) break;

        EnumDisplayMonitors(NULL, NULL, EnumFunction, 0);
        break;
    }
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

