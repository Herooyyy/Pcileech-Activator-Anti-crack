#include <windows.h>
#include <commctrl.h>
#include <FTD3XX.h>
#include <string>

#pragma comment(lib, "comctl32.lib")

// Window handles
HWND hwndMain;
HWND hwndEdit;
HWND hwndSendButton;
HWND hwndStatus;
FT_HANDLE ftHandle = nullptr;

// Window procedure
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_CREATE: {
            // Create input edit control
            hwndEdit = CreateWindowW(L"EDIT", L"0xDEADBEEF",
                WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL,
                10, 10, 200, 25, hwnd, NULL, GetModuleHandle(NULL), NULL);

            // Create send button
            hwndSendButton = CreateWindowW(L"BUTTON", L"发送激活信号",
                WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                220, 10, 100, 25, hwnd, (HMENU)1, GetModuleHandle(NULL), NULL);

            // Create status bar
            hwndStatus = CreateWindowExW(0, STATUSCLASSNAMEW, NULL,
                WS_CHILD | WS_VISIBLE | SBARS_SIZEGRIP,
                0, 0, 0, 0, hwnd, NULL, GetModuleHandle(NULL), NULL);

            // Initialize FTD3XX
            FT_STATUS ftStatus = FT_Create(0, FT_OPEN_BY_INDEX, &ftHandle);
            if (ftStatus != FT_OK) {
                SendMessageW(hwndStatus, SB_SETTEXT, 0, (LPARAM)L"错误: 无法连接到设备");
                EnableWindow(hwndSendButton, FALSE);
            } else {
                SendMessageW(hwndStatus, SB_SETTEXT, 0, (LPARAM)L"已连接到设备");
            }
            return 0;
        }

        case WM_COMMAND:
            if (LOWORD(wParam) == 1) { // Send button clicked
                if (ftHandle == nullptr) {
                    MessageBoxW(hwnd, L"设备未连接", L"错误", MB_ICONERROR);
                    return 0;
                }

                // Get input text
                wchar_t buffer[32];
                GetWindowTextW(hwndEdit, buffer, 32);

                // Convert hex string to value
                DWORD value;
                if (swscanf_s(buffer, L"0x%x", &value) != 1) {
                    MessageBoxW(hwnd, L"请输入有效的十六进制值 (例如: 0xDEADBEEF)", L"错误", MB_ICONERROR);
                    return 0;
                }

                // Send the value
                ULONG bytesWritten;
                FT_STATUS ftStatus = FT_WritePipe(ftHandle, 0x02, (PUCHAR)&value, sizeof(value), &bytesWritten, NULL);
                
                if (ftStatus == FT_OK && bytesWritten == sizeof(value)) {
                    SendMessageW(hwndStatus, SB_SETTEXT, 0, (LPARAM)L"激活信号已发送");
                } else {
                    SendMessageW(hwndStatus, SB_SETTEXT, 0, (LPARAM)L"发送失败");
                }
            }
            return 0;

        case WM_DESTROY:
            if (ftHandle != nullptr) {
                FT_Close(ftHandle);
            }
            PostQuitMessage(0);
            return 0;
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow) {
    // Initialize common controls
    INITCOMMONCONTROLSEX icc;
    icc.dwSize = sizeof(INITCOMMONCONTROLSEX);
    icc.dwICC = ICC_BAR_CLASSES;
    InitCommonControlsEx(&icc);

    // Register window class
    WNDCLASSEXW wc = {0};
    wc.cbSize = sizeof(WNDCLASSEXW);
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.lpszClassName = L"DMAActivator";
    RegisterClassExW(&wc);

    // Create main window
    hwndMain = CreateWindowExW(
        0,
        L"DMAActivator",
        L"DMA 激活器",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        350, 120,
        NULL, NULL, hInstance, NULL
    );

    ShowWindow(hwndMain, nCmdShow);
    UpdateWindow(hwndMain);

    // Message loop
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return (int)msg.wParam;
}
