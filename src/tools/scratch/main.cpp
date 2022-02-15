// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.
#include "../../inc/LibraryIncludes.h"

#include <windows.h>

struct handle_data
{
    unsigned long process_id;
    HWND window_handle;
};

BOOL is_main_window(HWND handle)
{
    auto owner{ GetWindow(handle, GW_OWNER) };
    // wprintf(fmt::format(L"\t\towner: {}\n", reinterpret_cast<unsigned long long>(owner)).c_str());
    return owner == (HWND)0 && IsWindowVisible(handle);
}

BOOL CALLBACK enum_windows_callback(HWND handle, LPARAM lParam)
{
    // wprintf(fmt::format(L"enum_windows_callback\n").c_str());
    handle_data& data = *(handle_data*)lParam;
    unsigned long process_id = 0;
    GetWindowThreadProcessId(handle, &process_id);
    const bool pidDidntMatch{ data.process_id != process_id };
    // wprintf(fmt::format(L"\tpidDidntMatch: {}\n", pidDidntMatch).c_str());
    const bool wasntMainWindow{ !is_main_window(handle) };
    // wprintf(fmt::format(L"\twasntMainWindow: {}\n", wasntMainWindow).c_str());
    if (pidDidntMatch || wasntMainWindow)
    {
        return TRUE;
    }

    wprintf(fmt::format(L"\tpidDidntMatch: {}\n", pidDidntMatch).c_str());
    wprintf(fmt::format(L"\twasntMainWindow: {}\n", wasntMainWindow).c_str());

    auto owner{ GetWindow(handle, GW_OWNER) };
    wprintf(fmt::format(L"\t\thandle: {}\n", reinterpret_cast<unsigned long long>(handle)).c_str());
    wprintf(fmt::format(L"\t\towner: {}\n", reinterpret_cast<unsigned long long>(owner)).c_str());

    data.window_handle = handle;
    return FALSE;
}

HWND find_main_window(unsigned long process_id)
{
    handle_data data;
    data.process_id = process_id;
    data.window_handle = 0;
    EnumWindows(enum_windows_callback, (LPARAM)&data);
    return data.window_handle;
}

static LRESULT __stdcall WndProc(HWND const window, UINT const message, WPARAM const wparam, LPARAM const lparam) noexcept
{
    switch (message)
    {
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProc(window, message, wparam, lparam);
}

// This wmain exists for help in writing scratch programs while debugging.
int __cdecl wmain(int /*argc*/, WCHAR* /*argv[]*/)
{
    const auto pid{ GetCurrentProcessId() };
    const auto dumb{ GetConsoleWindow() };

    wprintf(fmt::format(L"pid: {}\n", pid).c_str());
    wprintf(fmt::format(L"dumb: {}\n", reinterpret_cast<unsigned long long>(dumb)).c_str());

    const auto mainHwnd{ find_main_window(pid) };
    wprintf(fmt::format(L"mainHwnd: {}\n", reinterpret_cast<unsigned long long>(mainHwnd)).c_str());

    // Register the window class.
    const wchar_t CLASS_NAME[] = L"Sample Window Class";

    WNDCLASS wc = {};
    const auto hInst{ GetModuleHandle(NULL) };
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInst;
    wc.lpszClassName = CLASS_NAME;

    RegisterClass(&wc);

    // Create the window.

    HWND hwnd = CreateWindowEx(
        0, // Optional window styles.
        CLASS_NAME, // Window class
        L"Learn to Program Windows", // Window text
        WS_OVERLAPPEDWINDOW, // Window style

        // Size and position
        200,
        200,
        200,
        200,

        NULL, // Parent window
        NULL, // Menu
        hInst, // Instance handle
        NULL // Additional application data
    );

    wprintf(fmt::format(L"hwnd: {}\n", reinterpret_cast<unsigned long long>(hwnd)).c_str());

    const auto newHwnd{ find_main_window(pid) };
    wprintf(fmt::format(L"newHwnd: {}\n", reinterpret_cast<unsigned long long>(newHwnd)).c_str());

    if (hwnd == NULL)
    {
        return 0;
    }

    ShowWindow(hwnd, SW_SHOW);
    MSG msg = {};
    while (GetMessage(&msg, NULL, 0, 0) > 0)
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    wprintf(fmt::format(L"window was closed\n").c_str());

    
    wprintf(fmt::format(L"Opening a messagebox...\n").c_str());
    MessageBoxW(NULL, L"foo", L"bar", MB_OK);
    wprintf(fmt::format(L"closed a messagebox\n").c_str());


    return 0;
}
