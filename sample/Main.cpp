#if WIN32

#include <Windows.h>
#include <tchar.h> // for _tWinMain (ANSI: WinMain, Unicode: wWinMain)

#include "application/Application.h"

#pragma comment(linker,                                     \
                "/manifestdependency:\""                    \
                "type='win32' "                             \
                "name='Microsoft.Windows.Common-Controls' " \
                "version='6.0.0.0' "                        \
                "processorArchitecture='*' "                \
                "publicKeyToken='6595b64144ccf1df' "        \
                "language='*'\"")

LRESULT CALLBACK WndMsgProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

void BeginProc(HWND hwnd);
void UpdateProc();
void EndProc();

#ifdef AU_OPT_WINAPP_WINDOW
INT WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PTSTR pCmdLine, INT nCmdShow)
#else
int main(int argc, char* argv[])
#endif
{
    AU_LOG_TAG(AppDemo);

    AU_LOG_I(TAG, "=== ^_^AuBomb Application! ===");

    #if defined(DEBUG) || defined(_DEBUG)
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
    AU_LOG_D(TAG, "Enable runtime memory check for debug builds.");
    #endif

    AU_LOG_I(TAG, "Initialize window.");
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
        wndclass.lpszClassName = TAG;

        if (!RegisterClass(&wndclass)) {
            AU_LOG_F(TAG, "Register main window class failed, requires at least WindowsNT!");
            MessageBox(NULL, TEXT("Regist main window class failed, requires at least WindowsNT!"),
                TEXT("AuBomb"), MB_ICONERROR | MB_OK);
            return 0;
        }

        hwnd = CreateWindow(TAG, TEXT("AuBomb"), WS_OVERLAPPEDWINDOW,
            CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
            GetDesktopWindow(), NULL, hInstance, NULL);
        if (!hwnd) {
            AU_LOG_F(TAG, "Create application window failed!");
            MessageBox(NULL, TEXT("Create application window failed!"),
                TEXT("AuBomb"), MB_ICONERROR | MB_OK);
            return 0;
        }

        ShowWindow(hwnd, SW_SHOW);
        UpdateWindow(hwnd);
    }

    AU_LOG_I(TAG, "Program BEGIN running.");
    BeginProc(hwnd);
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
                UpdateProc();
            }
        }
    }
    EndProc();
    AU_LOG_I(TAG, "Program END running.");
    return static_cast<int>(msg.wParam);
}

LRESULT WndMsgProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    au::application::AppFw_Win32(hwnd, message, wParam, lParam);

    switch (message) {
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }

    return DefWindowProc(hwnd, message, wParam, lParam);
}

#include "Demo_00_Simple.h"
#include "Demo_01_Backend.h"
#include "Demo_02_Passflow.h"
#include "Demo_03_ShaderToy.h"

namespace {
std::unique_ptr<DemosBase> g_demo;
}

void BeginProc(HWND hwnd)
{
    std::string envIndex = "1";
    if (!au::application::Application::GetReference().GetEnv("DemoIndex", envIndex)) {
        AU_LOG_I("DemoIndex", "DemoIndex is not set, use default Demo_01_Backend(1).");
    }
    int index = std::stoi(envIndex);

    switch (index) {
        default:
            AU_LOG_W("DemoIndex", "Invalid demo index %d, use default Demo_00_Simple(0).", index);
            [[fallthrough]];

        case 0: { // Demo_00_Simple
            au::application::Application::GetReference().Setup(
                au::application::Application::AppType::Runtime,
                au::application::Application::AppRole::None);
            g_demo = std::make_unique<Demo_00_Simple>(
                au::application::Application::GetReference().AcquireScene(
                    au::application::Application::DEFAULT_SCENE_NAME));
            g_demo->Setup();
        }
        break;

        case 1: { // Demo_01_Backend
            au::application::Application::GetReference().Setup(
                au::application::Application::AppType::Runtime,
                au::application::Application::AppRole::None);
            g_demo = std::make_unique<Demo_01_Backend>();
            g_demo->Setup();
        }
        break;

        case 2: { // Demo_02_Passflow
            au::application::Application::GetReference().Setup(
                au::application::Application::AppType::Runtime,
                au::application::Application::AppRole::None);
            g_demo = std::make_unique<Demo_02_Passflow>();
            g_demo->Setup();
        }
        break;

        case 3: { // Demo_03_ShaderToy
            au::application::Application::GetReference().SetEnv("project", "appdemo/ShaderToy");
            au::application::Application::GetReference().Setup(
                au::application::Application::AppType::Runtime,
                au::application::Application::AppRole::None);

            auto& scene = au::application::Application::GetReference().AcquireScene(
                au::application::Application::DEFAULT_SCENE_NAME);

            au::application::AppFw_Win32_BindWindowAndRegistry(hwnd, scene.AcquireRegistry());

            auto demo = std::make_unique<Demo_03_ShaderToy>(scene);
            std::string aufdbr;
            if (!au::application::Application::GetReference().GetEnv("aufdbr", aufdbr)) {
                AU_LOG_W("DemoIndex=3", "Not exist aufdbr config, use default: SimpleRGB.aufdbr");
                aufdbr = "SimpleRGB.aufdbr";
            }
            demo->Setup();
            demo->SetFDBR(aufdbr);
            g_demo = std::move(demo);
        }
        break;
    }
}

void UpdateProc()
{
    au::application::Application::GetReference().Tick();
}

void EndProc()
{
    g_demo->Shutdown();

    au::application::Application::GetReference().ClearAllScene();
    au::application::Application::GetReference().Close();
}

#else

#include <iostream>

int main(int argc, char* argv[])
{
    std::cout << "DEMO NO SUPPORT YET" << std::endl;
}

#endif
