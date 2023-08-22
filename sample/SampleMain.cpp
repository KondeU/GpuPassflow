#include "MemoryLeakChecker.h"
#include "PassflowRP.h"
#include "PassflowCP.h"

int ErrorHandlerCallback(const char* args[3])
{
    const char * level = args[0], * tag = args[1], * content = args[2];
    #ifdef _MSC_VER
    printf_s
    #else
    printf
    #endif
    ("[%s]:[%s] %s\n", level, tag, content);
    return 0;
}

namespace {
std::unique_ptr<PassflowDemo> passflowDemo;
}

#if WIN32

#include <Windows.h>
#include <tchar.h> // for _tWinMain (ANSI: WinMain, Unicode: wWinMain)

#pragma comment(linker,                                     \
                "/manifestdependency:\""                    \
                "type='win32' "                             \
                "name='Microsoft.Windows.Common-Controls' " \
                "version='6.0.0.0' "                        \
                "processorArchitecture='*' "                \
                "publicKeyToken='6595b64144ccf1df' "        \
                "language='*'\"")

LRESULT CALLBACK WndMsgProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

#ifdef GP_DEMO_WINAPP_WINDOW
INT WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PTSTR pCmdLine, INT nCmdShow)
#else
int main(int argc, char* argv[])
#endif
{
    au::gp::ErrorHandler::RegisterHandler(ErrorHandlerCallback);

    GP_LOG_TAG(GpuPassflowDemo);

    GP_LOG_I(TAG, "=== ^_^GpuPassflow Demo Application! ===");

    #if defined(DEBUG) || defined(_DEBUG)
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
    GP_LOG_D(TAG, "Enable runtime memory check for debug builds.");
    #endif

    GP_LOG_I(TAG, "Initialize passflow demo.");
    // Parse environment configurations from command line.
    unsigned int selector = 0;
    {
        int argc = 0;
        LPWSTR* argv = NULL;
        argv = CommandLineToArgvW(GetCommandLineW(), &argc);
        if (argv != NULL) {
            for (int argn = 1; argn < argc; argn++) {
                std::string command = std::to_string(argv[argn]);
                if (command == "RP") {
                    selector = 0;
                } else if (command == "CP") {
                    selector = 1;
                }
            }
            LocalFree(argv);
        }
    }
    switch (selector) {
    default:
    case 0:
        passflowDemo = std::make_unique<PassflowRP>();
        break;
    case 1:
        passflowDemo = std::make_unique<PassflowCP>();
        break;
    }
    passflowDemo->Setup();

    GP_LOG_I(TAG, "Initialize window.");
    MSG msg{};
    HWND hwnd{};
    // GetModuleHandle(NULL) will return HMODULE which we can interpret as a HINSTANCE.
    // If call it from a dll, it will return the instance of the exe that has loaded
    // the dll, not of the dll itself. Here we just want to get a exe instance.
    HINSTANCE hInstance = GetModuleHandle(NULL);
    {
        WNDCLASS wndclass{};
        wndclass.style = CS_HREDRAW | CS_VREDRAW;
        wndclass.lpfnWndProc = WndMsgProc;
        wndclass.cbClsExtra = 0;
        wndclass.cbWndExtra = 0;
        wndclass.hInstance = hInstance;
        wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
        wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
        wndclass.hbrBackground = (HBRUSH)GetStockObject(LTGRAY_BRUSH);
        wndclass.lpszMenuName = NULL;
        wndclass.lpszClassName = TEXT("DEMO");

        if (!RegisterClass(&wndclass)) {
            GP_LOG_F(TAG, "Register main window class failed, requires at least WindowsNT!");
            MessageBox(NULL, TEXT("Regist main window class failed, requires at least WindowsNT!"),
                TEXT("DEMO"), MB_ICONERROR | MB_OK);
            return 0;
        }

        hwnd = CreateWindow(TEXT("DEMO"), TEXT("DEMO"), WS_OVERLAPPEDWINDOW,
            CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
            GetDesktopWindow(), NULL, hInstance, NULL);
        if (!hwnd) {
            GP_LOG_F(TAG, "Create application window failed!");
            MessageBox(NULL, TEXT("Create application window failed!"),
                TEXT("DEMO"), MB_ICONERROR | MB_OK);
            return 0;
        }

        ShowWindow(hwnd, SW_SHOW);
        UpdateWindow(hwnd);
    }

    GP_LOG_I(TAG, "Program BEGIN running.");
    {
        // Default window size, this is window size, and is not swapchain size!
        RECT rect{};
        GetWindowRect(hwnd, &rect);
        MoveWindow(hwnd, rect.left, rect.top, 800, 600, TRUE);
        // Message loop to process windows message and draw each frame.
        while (msg.message != WM_QUIT) {
            if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            } else {
                if (passflowDemo) {
                    passflowDemo->ExecuteOneFrame();
                }
            }
        }
    }
    GP_LOG_I(TAG, "Program END running.");
    return static_cast<int>(msg.wParam);
}

LRESULT WndMsgProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message) {
    case WM_SIZE:
        if (passflowDemo) {
            passflowDemo->SizeChanged(hwnd,
                static_cast<unsigned int>(LOWORD(lParam)),
                static_cast<unsigned int>(HIWORD(lParam)));
        }
        return 0;

    case WM_DESTROY:
        passflowDemo.reset();
        PostQuitMessage(0);
        return 0;
    }

    return DefWindowProc(hwnd, message, wParam, lParam);
}

#else

#include <iostream>

int main(int argc, char* argv[])
{
    std::cout << "DEMO NO SUPPORT YET" << std::endl;
}

#endif
